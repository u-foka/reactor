#ifndef __IWS_REACTOR_REACTOR_HPP__
#define __IWS_REACTOR_REACTOR_HPP__

#include <atomic>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <typeindex>
#include <vector>

#include "addon.hpp"
#include "addon_base.hpp"
#include "addon_filter.hpp"
#include "addon_filter_base.hpp"
#include "addon_func_map.hpp"
#include "callback_holder.hpp"
#include "contract_base.hpp"
#include "factory_base.hpp"
#include "not_registred_exception.hpp"
#include "priorities.hpp"
#include "type_already_registred_exception.hpp"
#include "typed_contract.hpp"

namespace iws::reactor {

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
   typedef std::vector<contract_base *> contract_list;

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
   void unregister_addon(const std::string &instance, priorities priority, const std::type_info &type);

   void register_addon_filter(
         const std::string &instance, priorities priority, std::unique_ptr<addon_filter_base> &&filter);
   void unregister_addon_filter(const std::string &instance, priorities priority, const std::type_info &type);

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
};

// ----

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
         throw factory_not_registred_exception(t, id.second);
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
   auto oi = detail::find_if(_object_list, [&obj](const std::shared_ptr<void> &item) { return &obj == item.get(); });

   if (oi != _object_list.end())
   {
      return std::static_pointer_cast<T>(*oi);
   }

   throw std::runtime_error("Object not found");
}

template<typename T>
typename addon_func_map<T>::type reactor::get_addons(const std::string &instance)
{
   typename addon_func_map<T>::type result;

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

} // namespace iws::reactor

#endif // __IWS_REACTOR_REACTOR_HPP__
