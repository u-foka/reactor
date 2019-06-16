#include "reactor.hpp"

#include <sstream>

namespace iws {

// Memory for the reactor object
static typename std::aligned_storage<sizeof(reactor), alignof(reactor)>::type r_memory;
reactor &r = reinterpret_cast<reactor &>(r_memory);

// This is done before the dynamic initialization is started since
// std::atomic<int> has a trivial constructor and initialized with a constexpr
std::atomic<int> reactor::init::instance_count(0);

reactor::reactor()
      : _shutting_down(false)
{
}

reactor::~reactor()
{
   std::unique_lock<std::recursive_mutex> reset_objects_lock(_reset_objects_mutex);
   _shutting_down = true;

   std::unique_lock<std::shared_mutex> factory_write_lock(_factory_mutex);
   _factory_map.clear();
   factory_write_lock.unlock();

   reset_objects();
}

reactor::factory_base::factory_base(const std::type_info &type)
      : _type(type)
{
}

reactor::factory_base::~factory_base() {}

const std::type_info &reactor::factory_base::get_type() const
{
   return _type;
}

void reactor::register_factory(
      const std::string &instance, priorities priority, std::unique_ptr<reactor::factory_base> &&factory)
{
   std::unique_lock<std::shared_mutex> factory_write_lock(_factory_mutex);

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
      prio_map.insert({priority, std::move(factory)});
   }
   else
   {
      // Otherwise raise an error
      throw type_already_registred_exception(factory->get_type(), instance, priority);
   }
}

void reactor::unregister_factory(const std::string &instance, priorities priority, const std::type_info &type)
{
   std::unique_lock<std::shared_mutex> factory_writelock(_factory_mutex);

   const index id(type, instance);

   auto it = _factory_map.find(id);
   if (it == _factory_map.end())
   {
      // No factory found for the given parameters
      throw type_not_registred_exception(type, instance);
   }

   auto &prio_map = it->second;

   auto it_prio = prio_map.find(priority);
   if (it_prio == prio_map.end())
   {
      // No factory found for the given parameters
      throw type_not_registred_exception(type, instance);
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
   std::unique_lock<std::shared_mutex> addon_write_lock(_addon_mutex);

   const index id(addon->get_type(), instance);
   _addon_map[id].insert({priority, std::move(addon)});
}

void reactor::register_addon_filter(
      const std::string &instance, priorities priority, std::unique_ptr<addon_filter_base> &&filter)
{
   std::unique_lock<std::shared_mutex> addon_write_lock(_addon_mutex);

   const index id(filter->get_type(), instance);
   _addon_filter_map[id].insert({priority, std::move(filter)});
}

void reactor::reset_objects()
{
   std::unique_lock<std::recursive_mutex> reset_objects_lock(_reset_objects_mutex);

   sig_before_reset_objects();

   // Do not change the locking order!
   // get() locks the map and list in this order, so we do the same to avoid dead locks
   std::unique_lock<std::recursive_mutex> object_list_lock(_object_list_mutex);
   std::unique_lock<std::shared_mutex> object_map_write_lock(_object_map_mutex);

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

reactor::type_not_registred_exception::type_not_registred_exception(const std::type_info &type, const std::string &name)
      : type(type)
      , name(name)
{
}

const char *reactor::type_not_registred_exception::what() const noexcept
{
   std::stringstream ss;
   ss << "There is no factory with the given type or name registred: " << type.name() << ", '" << name << "'";
   msg_buffer = ss.str();

   return msg_buffer.c_str();
}

reactor::type_already_registred_exception::type_already_registred_exception(
      const std::type_info &type, const std::string &name, priorities priority)
      : type(type)
      , name(name)
      , priority(priority)
{
}

const char *reactor::type_already_registred_exception::what() const noexcept
{
   std::stringstream ss;
   ss << "There is already a factory with the given type, name and priority registred: " << type.name() << ", '" << name
      << "', " << priority;
   msg_buffer = ss.str();

   return msg_buffer.c_str();
}

reactor::contract_base::contract_base() {}

reactor::contract_base::~contract_base() {}

reactor::init::init()
{
   if (1 != ++instance_count)
      return;

   // Placement new for the global instance
   new (&r) reactor();
}

reactor::init::~init()
{
   if (0 != --instance_count)
      return;

   // Destruct global instance
   r.~reactor();
}

} // namespace iws
