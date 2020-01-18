// Copyright 2020 Tamas Eisenberger <e.tamas@iwstudio.hu>
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

#include <reactor/reactor.hpp>

namespace iws {
namespace reactor {

static const std::string REACTOR_VERSION = MACRO_STR(PROJECT_VERSION);

reactor::reactor()
      : _shutting_down(false)
{
}

reactor::~reactor()
{
   std::unique_lock<std::recursive_mutex> reset_objects_lock(_reset_objects_mutex);
   _shutting_down = true;

   std::unique_lock<pf::might_shared_mutex> factory_write_lock(_factory_mutex);
   _factory_map.clear();
   factory_write_lock.unlock();

   reset_objects();
}

void reactor::register_factory(
      const std::string &instance, priorities priority, const std::shared_ptr<factory_base> &factory)
{
   std::unique_lock<pf::might_shared_mutex> factory_write_lock(_factory_mutex);

   const index id(factory->get_type(), instance);
   auto it = _factory_map.find(id);

   // Insert the type - name indexed item into the map if it not already exists
   if (it == _factory_map.end())
   {
      auto ret = _factory_map.insert(factory_map::value_type(id, priorities_map()));
      it = ret.first;
   }

   auto &prio_map = it->second;

   // Check if there is already a factory with the given priority
   auto it_prio = prio_map.find(priority);
   if (it_prio == prio_map.end())
   {
      // If there is no, insert the new
      prio_map.insert({priority, factory});
   }
   else
   {
      // Otherwise raise an error
      throw type_already_registred_exception(factory->get_type(), instance, priority);
   }
}

void reactor::unregister_factory(const std::string &instance, priorities priority, const std::type_info &type)
{
   std::unique_lock<pf::might_shared_mutex> factory_write_lock(_factory_mutex);

   const index id(type, instance);

   auto it = _factory_map.find(id);
   if (it == _factory_map.end())
   {
      // No factory found for the given parameters
      throw factory_not_registred_exception(type, instance);
   }

   auto &prio_map = it->second;

   auto it_prio = prio_map.find(priority);
   if (it_prio == prio_map.end())
   {
      // No factory found for the given parameters
      throw factory_not_registred_exception(type, instance);
   }

   // Found the factory in prio_map, remove it!
   prio_map.erase(it_prio);

   if (prio_map.empty())
   {
      // The prio_map is empty, let's remove the item from the factory_map too
      _factory_map.erase(it);
   }
}

void reactor::register_addon(const std::string &instance, priorities priority, std::unique_ptr<addon_base> &&addon)
{
   std::unique_lock<pf::might_shared_mutex> addon_write_lock(_addon_mutex);

   const index id(addon->get_type(), instance);
   _addon_map[id].insert({priority, std::move(addon)});
}

void reactor::unregister_addon(const std::string &instance, priorities priority, const std::type_info &type)
{
   std::unique_lock<pf::might_shared_mutex> addon_write_lock(_addon_mutex);

   const index id(type, instance);

   auto it = _addon_map.find(id);
   if (it == _addon_map.end())
   {
      // No addon found for the given parameters
      throw addon_not_registred_exception(type, instance);
   }

   auto &prio_map = it->second;

   auto it_prio = prio_map.find(priority);
   if (it_prio == prio_map.end())
   {
      // No addon found for the given parameters
      throw addon_not_registred_exception(type, instance);
   }

   // Found the addon in prio_map, remove it!
   prio_map.erase(it_prio);

   if (prio_map.empty())
   {
      // The prio_map is empty, let's remove the item from the addon_map too
      _addon_map.erase(it);
   }
}

void reactor::register_addon_filter(
      const std::string &instance, priorities priority, std::unique_ptr<addon_filter_base> &&filter)
{
   std::unique_lock<pf::might_shared_mutex> addon_write_lock(_addon_mutex);

   const index id(filter->get_type(), instance);
   _addon_filter_map[id].insert({priority, std::move(filter)});
}

void reactor::unregister_addon_filter(const std::string &instance, priorities priority, const std::type_info &type)
{
   std::unique_lock<pf::might_shared_mutex> addon_write_lock(_addon_mutex);

   const index id(type, instance);

   auto it = _addon_filter_map.find(id);
   if (it == _addon_filter_map.end())
   {
      // No addon_filter found for the given parameters
      throw addon_filter_not_registred_exception(type, instance);
   }

   auto &prio_map = it->second;

   auto it_prio = prio_map.find(priority);
   if (it_prio == prio_map.end())
   {
      // No addon_filter found for the given parameters
      throw addon_filter_not_registred_exception(type, instance);
   }

   // Found the addon_filter in prio_map, remove it!
   prio_map.erase(it_prio);

   if (prio_map.empty())
   {
      // The prio_map is empty, let's remove the item from the addon_filter_map too
      _addon_filter_map.erase(it);
   }
}

void reactor::reset_objects()
{
   std::unique_lock<std::recursive_mutex> reset_objects_lock(_reset_objects_mutex);

   sig_before_reset_objects();

   // Do not change the locking order!
   // get() locks the map and list in this order, so we do the same to avoid dead locks
   std::unique_lock<std::recursive_mutex> object_list_lock(_object_list_mutex);
   std::unique_lock<pf::might_shared_mutex> object_map_write_lock(_object_map_mutex);

   // The map holds weak copies so we can simply clear it
   _object_map.clear();

   // Ensure reverse destruction order of the objects
   while (!_object_list.empty())
   {
      _object_list.pop_back();
   }

   object_map_write_lock.unlock();
   object_list_lock.unlock();

   if (!_shutting_down)
   {
      sig_after_reset_objects();
   }
}

bool reactor::validate_contracts()
{
   for (auto it = _contract_list.begin(); it != _contract_list.end(); ++it)
   {
      auto id = (*it)->get_index();
      auto fit = _factory_map.find(id);
      if (fit == _factory_map.end())
      {
         fit = _factory_map.find(index(id.first, std::string()));
         if (fit == _factory_map.end())
         {
            return false;
         }
      }
   }

   return true;
}

reactor::contract_list reactor::unsatisfied_contracts()
{
   contract_list list;

   for (auto it = _contract_list.begin(); it != _contract_list.end(); ++it)
   {
      auto id = (*it)->get_index();
      auto fit = _factory_map.find(id);
      if (fit == _factory_map.end())
      {
         fit = _factory_map.find(index(id.first, std::string()));
      }

      if (fit == _factory_map.end())
      {
         list.push_back(*it);
      }
   }

   return list;
}

bool reactor::is_shutting_down()
{
   return _shutting_down;
}

const std::string &reactor::get_version() const
{
   return REACTOR_VERSION;
}

void reactor::register_contract(contract_base *cont)
{
   _contract_list.push_back(cont);
}

void reactor::unregister_contract(contract_base *cont)
{
   auto it = find(_contract_list.begin(), _contract_list.end(), cont);
   if (it != _contract_list.end())
   {
      _contract_list.erase(it);
   }
}

} // namespace reactor
} // namespace iws
