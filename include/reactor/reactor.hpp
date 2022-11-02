// Copyright 2022 Tamas Eisenberger <e.tamas@iwstudio.hu>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __IWS_REACTOR_REACTOR_HPP__
#define __IWS_REACTOR_REACTOR_HPP__

#include <atomic>
#include <memory>
#include <mutex>
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
#include "might_shared_mutex.hpp"
#include "not_registred_exception.hpp"
#include "priorities.hpp"
#include "type_already_registred_exception.hpp"
#include "typed_contract.hpp"
#include "utils.hpp"
#include "id_holder.hpp"

namespace iws {
namespace reactor {

namespace pf = ::iws::polyfil;

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
   void register_factory(
         const std::string &instance, priorities priority, const std::shared_ptr<factory_base> &factory);

   /**
    * @brief unregister an alrady registered factory
    * 
    * @param instance should be the same value which is used to register the factory need to be unregistered.
    * @param priority should be the same value which is used to register the factory need to be unregistered.
    * @param type should match the value that the factory -which need to be unregistered- returns through get_type().
    */
   void unregister_factory(const std::string &instance, priorities priority, const std::type_info &type);

   /**
    * @brief registers a new addon. More on addons: //TODO link to the addon chapter...
    * 
    * @param instance the instance name that should use this addon.
    *          Can be empty that means the addon will be used by the default instance.
    * @param priority the priority of the addon (all addons will be called, the priority alters the ordering)
    * @param addon rvalue reference to an unique_ptr containing the addon
    * @return a registration id as size_t that can be used to unregister the addon just registered.
    */
   size_t register_addon(const std::string &instance, priorities priority, std::unique_ptr<addon_base> &&addon);

   /**
    * @brief unregister all alrady registered addons for a given instance + type combination.
    * 
    * @param instance should be the same value which is used to register the addons need to be unregistered.
    * @param type should match the value that the addons -which need to be unregistered- returns through get_type().
    * @return the number of unregistered addons as size_t
    */
   size_t unregister_addons(const std::string &instance, const std::type_info &type);

   /**
    * @brief unregister all alrady registered addons for a given instance + priority + type combination.
    * 
    * @param instance should be the same value which is used to register the addons need to be unregistered.
    * @param priority should be the same value which is used to register the addons need to be unregistered.
    * @param type should match the value that the addons -which need to be unregistered- returns through get_type().
    * @return the number of unregistered addons as size_t
    */
   size_t unregister_addons(const std::string &instance, priorities priority, const std::type_info &type);

   /**
    * @brief unregister an already registered addon
    * 
    * @param instance should be the same value which is used to register the addon need to be unregistered.
    * @param type should match the value that the addon -which need to be unregistered- returns through get_type().
    * @param reg_id should match the registration id returned by register_addon when registering the addon.
    */
   void unregister_addon(const std::string &instance, const std::type_info &type, size_t reg_id);

   /**
    * @brief registers a new addon filter. More on addons: //TODO link to the addon chapter...
    * 
    * @param instance the instance name that should use this addon filter.
    *          Can be empty that means the addon will be used by the default instance.
    * @param priority the priority of the addon (all addons will be called, the priority alters the ordering)
    * @param filter rvalue reference to an unique_ptr containing the addon filter
    * @return a registration id as size_t that can be used to unregister the addon just registered.
    */
   size_t register_addon_filter(
         const std::string &instance, priorities priority, std::unique_ptr<addon_filter_base> &&filter);

   /**
    * @brief unregister all alrady registered addon filters for a given instance + type combination.
    * 
    * @param instance should be the same value which is used to register the addon filters need to be unregistered.
    * @param type should match the value that the addon filters -which need to be unregistered- returns through
    *          get_type().
    * @return the number of unregistered addon filters as size_t
    */
   size_t unregister_addon_filters(const std::string &instance, const std::type_info &type);

   /**
    * @brief unregister all alrady registered addon filters for a given instance + priority + type combination.
    * 
    * @param instance should be the same value which is used to register the addon filters need to be unregistered.
    * @param priority should be the same value which is used to register the addon filters need to be unregistered.
    * @param type should match the value that the addon filters -which need to be unregistered- returns through
    *          get_type().
    * @return the number of unregistered addon filters as size_t
    */
   size_t unregister_addon_filters(const std::string &instance, priorities priority, const std::type_info &type);

   /**
    * @brief unregister an already registered addon filter
    * 
    * @param instance should be the same value which is used to register the addon filter need to be unregistered.
    * @param type should match the value that the addon filter -which need to be unregistered- returns through
    *          get_type().
    * @param reg_id should match the registration id returned by register_addon_filter when registering the
    *          addon filter.
    */
   void unregister_addon_filter(const std::string &instance, const std::type_info &type, size_t reg_id);


   template<typename T>
   bool instance_exists(const typed_contract<T> &contract) const;
   template<typename T>
   T &get(const typed_contract<T> &contract);
   template<typename T>
   std::shared_ptr<T> get_ptr(T &obj);
   void reset_objects();

   template<typename T>
   typename addon_func_map<T>::type get_addons(const std::string &instance = std::string()) const;

   threadsafe_callback_holder<> sig_before_reset_objects;
   threadsafe_callback_holder<> sig_after_reset_objects;

   bool validate_contracts() const;
   contract_list unsatisfied_contracts() const;
   void test_all_contracts() const;

   bool is_shutting_down() const;

   const std::string &get_version() const;

 private:
   typedef std::map<priorities, std::shared_ptr<factory_base>>
         priorities_map; // Must be shared_ptr so get() can safely
                         // release the factory read mutex while creating the object to avoid recursive locking of the
                         // shared mutex
   typedef std::map<index, priorities_map> factory_map;
   typedef std::map<index, std::shared_ptr<void>> object_map;
   typedef std::vector<std::shared_ptr<void>> object_list;
   typedef std::vector<index> wip_list;
   typedef std::unique_ptr<addon_base> addon_ptr;
   typedef id_holder<size_t, addon_ptr> addon_holder;
   typedef std::multimap<priorities, addon_holder> addon_priority_map;
   typedef std::map<index, addon_priority_map> addon_map;
   typedef std::unique_ptr<addon_filter_base> addon_filter_ptr;
   typedef id_holder<size_t, addon_filter_ptr> addon_filter_holder;
   typedef std::multimap<priorities, addon_filter_holder> addon_filter_priority_map;
   typedef std::map<index, addon_filter_priority_map> addon_filter_map;

   factory_map _factory_map;
   object_map _object_map;
   object_list _object_list;
   wip_list _wip_list;
   contract_list _contract_list;
   addon_map _addon_map;
   std::atomic_size_t _addon_id;
   addon_filter_map _addon_filter_map;
   std::atomic_size_t _addon_filter_id;

   pf::might_shared_mutex _factory_mutex;
   mutable pf::might_shared_mutex _addon_mutex;
   mutable pf::might_shared_mutex _object_map_mutex;
   std::recursive_mutex _object_list_mutex; // also protects wip_list
   std::recursive_mutex _reset_objects_mutex;
   mutable std::mutex _contract_mutex;

   std::atomic_bool _shutting_down;

   void register_contract(contract_base *cont);
   void unregister_contract(contract_base *cont);
   friend class contract_base;
};

// ----

template<typename T>
bool reactor::instance_exists(const typed_contract<T> &contract) const
{
   const index &id = contract.get_index();

   // Try to find an existing instance
   pf::might_shared_lock<pf::might_shared_mutex> object_map_read_lock(_object_map_mutex);
   auto oi = _object_map.find(id);

   return oi != _object_map.end();
}

template<typename T>
T &reactor::get(const typed_contract<T> &contract)
{
   const std::type_info &t = typeid(T);
   const index &id = contract.get_index();

   // Try to find an existing instance
   pf::might_shared_lock<pf::might_shared_mutex> object_map_read_lock(_object_map_mutex);
   auto oi = _object_map.find(id);
   if (oi != _object_map.end())
   {
      return *static_cast<T *>(oi->second.get());
   }
   // Release the shared lock so we (or another thread) can acquire the unique lock on the object map after producing
   // a new object
   object_map_read_lock.unlock();

   // The object has not yet been created, letcs look for it's factory
   pf::might_shared_lock<pf::might_shared_mutex> factory_read_lock(_factory_mutex);
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
   auto selected_factory = fii->second;

   // Unlock to avoid recursive locking of the shared mutex. The factory is held by a shared_ptr so it won't disappear
   factory_read_lock.unlock();

   std::unique_lock<std::recursive_mutex> object_list_lock(_object_list_mutex);
   // Recheck if object were created since we've released the object read lock
   // Objects are only created and added while the object_list is locked
   oi = _object_map.find(id);
   if (oi != _object_map.end())
   {
      return *static_cast<T *>(oi->second.get());
   }

   //
   if (_wip_list.end() != std::find(_wip_list.begin(), _wip_list.end(), id))
   {
      throw std::runtime_error("Recursive call to reactor.get() on the same object");
   }
   _wip_list.push_back(id);

   try
   {
      // Call the factory to produce the requested object
      // Do this while only holding the recursive object list mutex so a constructor is able to recursively call get
      // to acquire it's dependencies
      auto obj = selected_factory->produce(id.second).get<T>();

      _wip_list.pop_back(); // No need to find, it has to be the back item :)

      // Also lock the map for actual insert
      // Don't lock earlies so getters of other types can still work while creating the object, and to allow recursion
      std::unique_lock<pf::might_shared_mutex> object_map_write_lock(_object_map_mutex);
      // Store the constructed object
      _object_map.insert(object_map::value_type(id, obj));
      _object_list.push_back(obj);

      return *static_cast<T *>(obj.get());
   }
   catch (...)
   {
      if (id == _wip_list.back()) // Crashed before popping the list..
      {
         _wip_list.pop_back(); // Just in case... ;)
      }
      throw;
   }
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
typename addon_func_map<T>::type reactor::get_addons(const std::string &instance) const
{
   pf::might_shared_lock<pf::might_shared_mutex> addon_read_lock(_addon_mutex);

   typename addon_func_map<T>::type result;

   auto it = _addon_map.find(index{typeid(T), instance});
   if (_addon_map.end() == it)
   {
      return result;
   }

   for (auto &item : it->second)
   {
      result.insert({item.first, dynamic_cast<addon<T> *>(item.second.value.get())});
   }

   auto it_filter = _addon_filter_map.find(index{typeid(T), instance});
   if (_addon_filter_map.end() != it_filter)
   {
      for (auto &filter : it_filter->second)
      {
         dynamic_cast<addon_filter<T> *>(filter.second.value.get())->filter_func(result);
      }
   }

   return result;
}

} // namespace reactor
} // namespace iws

#endif // __IWS_REACTOR_REACTOR_HPP__
