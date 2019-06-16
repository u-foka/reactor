#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <atomic>
#include <exception>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <tuple>
#include <typeindex>
#include <typeinfo>
#include <utility>

#include "callback_holder.hpp"
#include "reactor_utils.hpp"

namespace iws {

class reactor;
extern reactor &r;

/**
 * @brief Manages singleton objects referenced with interfaces and names.
 *
 * There is a global instance of reactor called "r". It is guaranteed to be available from the static init / deinit
 * state of tranlation units that include reactor.hpp.
 * Supports nameless "default" instances.
 * Interfaces with factories can be resigtered in the static init phase.
 * Objects are created when get() is first called for an interface and name pair.
 * All objects are destructed after the static destruction phase is complete for all the compilation units that
 * include reactor.h. The order of destruction if guaranteed to be 1the reverse of their creation.
 */
class reactor
{
 public:
   /**
    * @brief Enumeration holding possible priority levels
    */
   enum priorities
   {
      prio_fallback = 100,
      prio_normal = 200,
      prio_override = 300,
      prio_unittest = 400,
   };

   /**
    * @brief Link to a reactor managed instance
    */
   typedef std::pair<std::type_index, std::string> index;

   /**
    * @brief Wrapper for objects returned by factories
    *
    * Usin this object reactor can validate the type of the instances created by the factories while keeping the
    * generic interface.
    */
   class factory_result
   {
    public:
      factory_result() = delete;
      /**
       * @brief Construct a new factory_result object
       *
       * @param obj should be an shared pointer of type that the factory is registered for
       */
      template<typename T>
      factory_result(std::shared_ptr<T> obj);
      /**
       * @brief Gets the stored object
       *
       * @tparam T checks if the result object was created with exactly the same type of pointer
       * @return returns a void shared_ptr containing the constructed object after validating the type
       */
      template<typename T>
      std::shared_ptr<void> get();

    private:
      std::shared_ptr<void> _obj;
      std::type_index _id;
   };

   /**
    * @brief Interface for factories to be registred
    *
    * Usually you should not need to inherit facotry_base directly, you can use reactor::factory when you only need to
    * construct an object, or reactor::factory_wrapper if you want to add some logic.
    */
   class factory_base
   {
    public:
      /**
       * @brief factory_base constructor
       * @param type is the type_info of which type of objects the factory will produce.
       */
      factory_base(const std::type_info &type);
      virtual ~factory_base();
      /**
       * @brief get produced type
       * @return returns the type_info of which type of objects the factory will produce.
       */
      const std::type_info &get_type() const;
      /**
       * @brief produce a new object
       * @param instance is the name of the instance to be produced
       * @return returns a shared_ptr containing a new object of the factoryes type
       */
      virtual factory_result produce(const std::string &instance) const = 0;

    private:
      const std::type_info &_type;
   };

   /**
    * @brief default factory implementation
    *
    * This factory can construct any type, and return it downcasted as it's base class (preferably interface).
    * It holds the given constructor arguments, and passes them to the constructor of the given type.
    * It can also pass an instance name to the constructor as the first parameter.
    *
    * @tparam I The returned type (preferably an interface class).
    * @tparam T The constructed type.
    * @tparam pass_name If true, the constructor of T gets the instance name as it's first argument.
    * @tparam Args the types of constructor argumens.
    */
   template<typename I, typename T, bool pass_name, typename... Args>
   class factory : public factory_base
   {
    public:
      /**
       * @brief factory constructor
       * @param args arguments to be passed to the constructor of T.
       */
      factory(Args &&... args);
      /**
       * @brief produce a new object
       * @param instance is the name of the instance constructed
       *        (passed to the constructor of T if pass_name is true).
       * @return returns an shared_ptr holding an instance of T downcasted to I.
       */
      virtual factory_result produce(const std::string &instance) const;

    private:
      std::tuple<Args...> _args;

      template<bool do_pass_name, size_t... Idx>
      factory_result produce_impl(
            typename std::enable_if<!do_pass_name, const std::string &>::type, std::index_sequence<Idx...>) const;

      template<bool do_pass_name, size_t... Idx>
      factory_result produce_impl(typename std::enable_if<do_pass_name, const std::string &>::type instance,
            std::index_sequence<Idx...>) const;
   };

   /**
    * @brief factory wrapper for custom factory logic
    *
    * This factory is constructed with a functor that is called every time a new object has to created.
    * Thorugh the functor, any custom logic can be added to the object creation.
    *
    * Template parameters:
    * - I: The returned type (preferably an interface class).
    */
   template<typename I>
   class factory_wrapper : public factory_base
   {
    public:
      typedef std::function<std::shared_ptr<I>(const std::string &)> producer_function;
      /**
       * @brief factory_wrapper constructor
       * @param producer is the functor that creates a new I* object
       */
      factory_wrapper(const producer_function &producer);
      /**
       * @brief produces a new object
       * @param instance is the name of the instance to be created
       * @return returns a new object of type I wrapped into an shared_ptr
       */
      virtual factory_result produce(const std::string &instance) const;

    private:
      producer_function _producer;
   };

   /**
    * @brief registers a factory
    *
    * When constructed, creates a reactor::factory object, and registers it into the global reactor.
    * Can be used to register factories in static init.
    *
    * @tparam I The type returned by the factory (preferably an interface class).
    * @tparam T The type constructed by the factory.
    * @tparam pass_name If true, the constructor of T gets the instance name as it's first argument.
    */
   template<typename I, typename T, bool pass_name = false, bool unregister = false>
   class factory_registrator
   {
    public:
      /**
       * @brief factory_registrator default instance constructor
       * @param priority is the registration priority of the factory
       * @param args are passed to the constructor of T
       *
       * Use this overload to register a default factory.
       */
      template<typename... Args>
      factory_registrator(priorities priority, Args &&... args);

      /**
       * @brief factory_registrator instance specific constructor
       * @param instance is the name of the instance that the registered factory produces
       * @param priority is the registration priority of the factory
       * @param args are passed to the constructor of T
       *
       * Use this overload to register a factory for a given instance name only.
       */
      template<typename... Args>
      factory_registrator(const std::string &instance, priorities priority, Args &&... args);

      ~factory_registrator();

    private:
      const std::string _name;
      const priorities _priority;
   };

   /**
    * @brief registers a factory_wrapper
    *
    * When constructed, creates a reactor::factory_wrapper object, and registers it into the global reactor.
    * Can be used to register factories in static init.
    *
    * @tparam I The type returned by the factory (preferably an interface class).
    */
   template<typename I, bool unregister = false>
   class factory_wrapper_registrator
   {
    public:
      /**
       * @brief factory_wrapper_registrator default instance constructor
       * @param priority is the registration priority of the factory
       * @param producer is the functor used to do the actual construction
       *
       * Use this overload to register a default factory.
       */
      factory_wrapper_registrator(priorities priority, const typename factory_wrapper<I>::producer_function &producer);
      /**
       * @brief factory_wrapper_registrator instance specific constructor
       * @param instance is the name of the instance that the registered factory produces
       * @param priority is the registration priority of the factory
       * @param producer is the functor used to do the actual construction
       *
       * Use this overload to register a factory for a given instance name only.
       */
      factory_wrapper_registrator(const std::string &instance, priorities priority,
            const typename factory_wrapper<I>::producer_function &producer);

      ~factory_wrapper_registrator();

    private:
      const std::string _name;
      const priorities _priority;
   };

   class type_not_registred_exception : public std::exception
   {
    public:
      type_not_registred_exception(const std::type_info &type, const std::string &name);
      virtual const char *what() const noexcept;

    private:
      const std::type_info &type;
      const std::string name;
      mutable std::string msg_buffer;
   };

   class type_already_registred_exception : public std::exception
   {
    public:
      type_already_registred_exception(const std::type_info &type, const std::string &name, priorities priority);
      virtual const char *what() const noexcept;

    private:
      const std::type_info &type;
      const std::string name;
      const priorities priority;
      mutable std::string msg_buffer;
   };

   class contract_base
   {
    public:
      contract_base();
      virtual ~contract_base();
      virtual const index &get_index() const = 0;
   };
   typedef std::vector<contract_base *> contract_list;

   template<typename T>
   class typed_contract : public contract_base
   {
   };

   template<typename T>
   class contract : public typed_contract<T>
   {
    public:
      contract(const std::string &instance = std::string());
      contract(reactor *r_inst, const std::string &instance = std::string());
      virtual ~contract();
      virtual const index &get_index() const override;

    private:
      const index _index;
      reactor *const _r_inst;
   };

   class addon_base
   {
    public:
      virtual ~addon_base() {}
      virtual const std::type_info &get_type() const = 0;
      virtual const std::type_info &get_interface_type() const = 0;
   };

   template<typename T>
   class addon : public addon_base
   {
    public:
      addon(typename T::func &&addon_func);

      const typename T::func addon_func;
      virtual const std::type_info &get_type() const override;
      virtual const std::type_info &get_interface_type() const override;
   };

   template<typename T, bool unregister = false>
   class addon_registrator
   {
    public:
      addon_registrator(priorities priority, addon<T> &&inst);
      addon_registrator(const std::string &instance, priorities priority, addon<T> &&inst);

      ~addon_registrator();

    private:
      const std::string _name;
      const priorities _priority;
   };

   template<typename T>
   struct addon_func_map
   {
      typedef std::multimap<priorities, typename std::add_pointer<addon<T>>::type> type;
   };

   class addon_filter_base
   {
    public:
      virtual ~addon_filter_base() {}
      virtual const std::type_info &get_type() const = 0;
      virtual const std::type_info &get_interface_type() const = 0;
   };

   template<typename T>
   class addon_filter : public addon_filter_base
   {
    public:
      typedef std::function<void(typename addon_func_map<T>::type &)> func;

      addon_filter(func &&filter_func);

      const func filter_func;
      virtual const std::type_info &get_type() const override;
      virtual const std::type_info &get_interface_type() const override;
   };

   reactor();
   ~reactor();

   /**
    * @brief register a new factory
    * @param instance name of the object constructed by the registered factory.
    *          Can be empty that means registering the default factory for the given type.
    * @param priority of the registered factory. Factories with higher priority override ones with lower.
    * @param factory is the factory to be registered
    */
   void register_factory(const std::string &instance, priorities priority, std::unique_ptr<factory_base> &&factory);
   void unregister_factory(const std::string &instance, priorities priority, const std::type_info &type);

   void register_addon(const std::string &instance, priorities priority, std::unique_ptr<addon_base> &&addon);

   void register_addon_filter(
         const std::string &instance, priorities priority, std::unique_ptr<addon_filter_base> &&filter);

   template<typename T>
   T &get(const typed_contract<T> &contract);
   template<typename T>
   std::shared_ptr<T> get_ptr(T &obj);
   void reset_objects();

   template<typename T>
   typename addon_func_map<T>::type get_addons(const std::string &instance = std::string());

   callback_holder<> sig_before_reset_objects;
   callback_holder<> sig_after_reset_objects;

   bool validate_contracts();
   contract_list unsatisfied_contracts();

   bool is_shutting_down();

 private:
   typedef std::map<priorities, std::unique_ptr<factory_base>> priorities_map;
   typedef std::map<index, priorities_map> factory_map;
   typedef std::map<index, std::shared_ptr<void>> object_map;
   typedef std::vector<std::shared_ptr<void>> object_list;
   typedef std::multimap<priorities, std::unique_ptr<addon_base>> addon_priority_map;
   typedef std::map<index, addon_priority_map> addon_map;
   typedef std::multimap<priorities, std::unique_ptr<addon_filter_base>> addon_filter_priority_map;
   typedef std::map<index, addon_filter_priority_map> addon_filter_map;

   factory_map _factory_map;
   object_map _object_map;
   object_list _object_list;
   contract_list _contract_list;
   addon_map _addon_map;
   addon_filter_map _addon_filter_map;

   std::shared_mutex _factory_mutex;
   std::shared_mutex _addon_mutex;
   std::shared_mutex _object_map_mutex;
   std::recursive_mutex _object_list_mutex;
   std::recursive_mutex _reset_objects_mutex;

   std::atomic_bool _shutting_down;

   void register_contract(contract_base *cont);
   void unregister_contract(contract_base *cont);
   friend class contract_base;

 public:
   /**
    * @brief Go AWAY! Don't touch!
    *
    * Nifty Counter
    * This class is used to initialize the global "r" object.
    */
   class init
   {
    public:
      init();
      ~init();

    private:
      static std::atomic<int> instance_count;
   };
};

static const reactor::init __init_reactor;

//
// reactor
//

template<typename T>
T &reactor::get(const typed_contract<T> &contract)
{
   const std::type_info &t = typeid(T);
   const index &id = contract.get_index();

   // Try to find an existing instance
   std::shared_lock<std::shared_mutex> object_map_read_lock(_object_map_mutex);
   auto oi = _object_map.find(id);
   if (oi != _object_map.end())
   {
      return *static_cast<T *>(oi->second.get());
   }
   object_map_read_lock.unlock();

   // The object has not yet been created, letcs look for it's factory
   std::shared_lock<std::shared_mutex> factory_read_lock(_factory_mutex);
   auto fi = _factory_map.find(id);
   if (fi == _factory_map.end())
   {
      // Look for the default factory if there isn't a named one
      fi = _factory_map.find(index(t, std::string()));
      if (fi == _factory_map.end())
      {
         // No factory found for the given parameters
         throw type_not_registred_exception(t, id.second);
      }
   }

   // Get the factory with the highest priority
   auto fii = fi->second.rbegin();
   // No validity check here, register and unregister factory should make sure that the priority map always
   // has at least one item
   auto &selected_factory = fii->second;

   std::unique_lock<std::recursive_mutex> object_list_lock(_object_list_mutex);
   // Recheck if object were created since we've released the object read lock
   // Objects are only created and added while the object_list is locked
   oi = _object_map.find(id);
   if (oi != _object_map.end())
   {
      return *static_cast<T *>(oi->second.get());
   }

   // Call the factory to produce the requested object
   auto obj = selected_factory->produce(id.second).get<T>();

   // Also lock the map for actual insert
   std::unique_lock<std::shared_mutex> object_map_write_lock(_object_map_mutex);
   // Store the constructed object
   _object_map.insert(object_map::value_type(id, obj));
   _object_list.push_back(obj);

   return *static_cast<T *>(obj.get());
}

template<typename T>
std::shared_ptr<T> reactor::get_ptr(T &obj)
{
   std::unique_lock<std::recursive_mutex> object_list_lock(_object_list_mutex);

   // Try to find an existing instance
   auto oi = reactor_utils::find_if(_object_list, [&obj](const std::shared_ptr<void> &item) { return &obj == item.get(); });

   if (oi != _object_list.end())
   {
      return std::static_pointer_cast<T>(*oi);
   }

   throw std::runtime_error("Object not found");
}

template<typename T>
typename reactor::addon_func_map<T>::type reactor::get_addons(const std::string &instance)
{
   typename reactor::addon_func_map<T>::type result;

   for (auto &[prio, item] : _addon_map[index{typeid(T), instance}])
   {
      result.insert({prio, dynamic_cast<addon<T> *>(item.get())});
   }

   for (auto &filter : _addon_filter_map[index{typeid(T), instance}])
   {
      dynamic_cast<addon_filter<T> *>(filter.second.get())->filter_func(result);
   }

   return result;
}

//
// reactor::factory_result
//

template<typename T>
reactor::factory_result::factory_result(std::shared_ptr<T> obj)
      : _obj(obj)
      , _id(typeid(T))
{
}

template<typename T>
std::shared_ptr<void> reactor::factory_result::get()
{
   if (_id != typeid(T))
   {
      throw std::logic_error("Factory returned bad type");
   }

   return _obj;
}

//
// reactor::factory
//

template<typename I, typename T, bool pass_name, typename... Args>
reactor::factory<I, T, pass_name, Args...>::factory(Args &&... args)
      : factory_base(typeid(I))
      , _args(std::forward<Args>(args)...)
{
}

template<typename I, typename T, bool pass_name, typename... Args>
reactor::factory_result reactor::factory<I, T, pass_name, Args...>::produce(const std::string &instance) const
{
   return produce_impl<pass_name>(instance, std::index_sequence_for<Args...>());
}

template<typename I, typename T, bool pass_name, typename... Args>
template<bool do_pass_name, size_t... Idx>
reactor::factory_result reactor::factory<I, T, pass_name, Args...>::produce_impl(
      typename std::enable_if<!do_pass_name, const std::string &>::type, std::index_sequence<Idx...>) const
{
   return std::shared_ptr<I>(std::make_shared<T>(std::get<Idx>(_args)...));
}

template<typename I, typename T, bool pass_name, typename... Args>
template<bool do_pass_name, size_t... Idx>
reactor::factory_result reactor::factory<I, T, pass_name, Args...>::produce_impl(
      typename std::enable_if<do_pass_name, const std::string &>::type instance, std::index_sequence<Idx...>) const
{
   return std::shared_ptr<I>(std::make_shared<T>(instance, std::get<Idx>(_args)...));
}

//
// reactor::factory_wrapper
//

template<typename I>
reactor::factory_wrapper<I>::factory_wrapper(const producer_function &producer)
      : factory_base(typeid(I))
      , _producer(producer)
{
}

template<typename I>
reactor::factory_result reactor::factory_wrapper<I>::produce(const std::string &instance) const
{
   return _producer(instance);
}

//
// reactor::factory_registrator
//

template<typename I, typename T, bool pass_name, bool unregister>
template<typename... Args>
reactor::factory_registrator<I, T, pass_name, unregister>::factory_registrator(
      reactor::priorities priority, Args &&... args)
      : _name(std::string())
      , _priority(priority)
{
   r.register_factory(
         _name, _priority, std::make_unique<reactor::factory<I, T, pass_name, Args...>>(std::forward<Args>(args)...));
}

template<typename I, typename T, bool pass_name, bool unregister>
template<typename... Args>
reactor::factory_registrator<I, T, pass_name, unregister>::factory_registrator(
      const std::string &instance, reactor::priorities priority, Args &&... args)
      : _name(instance)
      , _priority(priority)
{
   r.register_factory(
         _name, _priority, std::make_unique<reactor::factory<I, T, pass_name, Args...>>(std::forward<Args>(args)...));
}

template<typename I, typename T, bool pass_name, bool unregister>
reactor::factory_registrator<I, T, pass_name, unregister>::~factory_registrator()
{
   if (unregister)
   {
      r.unregister_factory(_name, _priority, typeid(I));
   }
}

//
// reactor::factory_wrapper_registrator
//

template<typename I, bool unregister>
reactor::factory_wrapper_registrator<I, unregister>::factory_wrapper_registrator(
      priorities priority, const typename factory_wrapper<I>::producer_function &producer)
      : _name(std::string())
      , _priority(priority)
{
   r.register_factory(_name, _priority, std::make_unique<reactor::factory_wrapper<I>>(producer));
}

template<typename I, bool unregister>
reactor::factory_wrapper_registrator<I, unregister>::factory_wrapper_registrator(
      const std::string &instance, priorities priority, const typename factory_wrapper<I>::producer_function &producer)
      : _name(instance)
      , _priority(priority)
{
   r.register_factory(_name, _priority, std::make_unique<reactor::factory_wrapper<I>>(producer));
}

template<typename I, bool unregister>
reactor::factory_wrapper_registrator<I, unregister>::~factory_wrapper_registrator()
{
   if (unregister)
   {
      r.unregister_factory(_name, _priority, typeid(I));
   }
}

//
// reactor::contract
//

template<typename T>
reactor::contract<T>::contract(const std::string &instance)
      : _index(typeid(T), instance), _r_inst(&r)
{
   _r_inst->register_contract(this);
}

template<typename T>
reactor::contract<T>::contract(reactor *r_inst, const std::string &instance)
      : _index(typeid(T), instance), _r_inst(r_inst)
{
   _r_inst->register_contract(this);
}

template<typename T>
reactor::contract<T>::~contract()
{
   _r_inst->unregister_contract(this);
}

template<typename T>
const reactor::index &reactor::contract<T>::get_index() const
{
   return _index;
}

//
// reactor::addon
//

template<typename T>
reactor::addon<T>::addon(typename T::func &&addon_func)
      : addon_func(addon_func)
{
}

template<typename T>
const std::type_info &reactor::addon<T>::get_type() const
{
   return typeid(T);
}

template<typename T>
const std::type_info &reactor::addon<T>::get_interface_type() const
{
   return typeid(typename T::interface);
}

//
// reactor::addon_registrator
//

template<typename T, bool unregister>
reactor::addon_registrator<T, unregister>::addon_registrator(reactor::priorities priority, addon<T> &&inst)
      : _name(std::string())
      , _priority(priority)
{
   r.register_addon(_name, _priority, std::make_unique<addon<T>>(std::move(inst)));
}

template<typename T, bool unregister>
reactor::addon_registrator<T, unregister>::addon_registrator(const std::string &instance, reactor::priorities priority, addon<T> &&inst)
      : _name(instance)
      , _priority(priority)
{
   r.register_addon(_name, _priority, std::make_unique<addon<T>>(std::move(inst)));
}

template<typename T, bool unregister>
reactor::addon_registrator<T, unregister>::~addon_registrator()
{
   if (unregister)
   {
      // TODO: Finish addon managmement :)
      //r.unregister_addon(_name, _priority, typeid(I));
   }
}

//
// reactor::addon_filter
//

template<typename T>
reactor::addon_filter<T>::addon_filter(func &&filter_func)
      : filter_func(filter_func)
{
}

template<typename T>
const std::type_info &reactor::addon_filter<T>::get_type() const
{
   return typeid(T);
}

template<typename T>
const std::type_info &reactor::addon_filter<T>::get_interface_type() const
{
   return typeid(typename T::interface);
}

} // namespace iws

#endif // REACTOR_HPP
