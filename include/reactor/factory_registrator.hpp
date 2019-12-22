#ifndef __IWS_REACTOR_FACTORY_REGISTRATOR_HPP__
#define __IWS_REACTOR_FACTORY_REGISTRATOR_HPP__

#include <string>

#include "priorities.hpp"
#include "r.hpp"
#include "reactor.hpp"
#include "factory.hpp"

namespace iws::reactor {

/**
 * @brief registers a factory
 *
 * When constructed, creates a factory object, and registers it into the global reactor.
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

// ----

template<typename I, typename T, bool pass_name, bool unregister>
template<typename... Args>
factory_registrator<I, T, pass_name, unregister>::factory_registrator(priorities priority, Args &&... args)
      : _name(std::string())
      , _priority(priority)
{
   r.register_factory(
         _name, _priority, std::make_unique<factory<I, T, pass_name, Args...>>(std::forward<Args>(args)...));
}

template<typename I, typename T, bool pass_name, bool unregister>
template<typename... Args>
factory_registrator<I, T, pass_name, unregister>::factory_registrator(
      const std::string &instance, priorities priority, Args &&... args)
      : _name(instance)
      , _priority(priority)
{
   r.register_factory(
         _name, _priority, std::make_unique<factory<I, T, pass_name, Args...>>(std::forward<Args>(args)...));
}

template<typename I, typename T, bool pass_name, bool unregister>
factory_registrator<I, T, pass_name, unregister>::~factory_registrator()
{
   if (unregister)
   {
      r.unregister_factory(_name, _priority, typeid(I));
   }
}

} // namespace iws::reactor

#endif //__IWS_REACTOR_FACTORY_REGISTRATOR_HPP__
