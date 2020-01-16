// Copyright 2020 Tamás Eisenberger <e.tamas@iwstudio.hu>
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

#ifndef __IWS_REACTOR_FACTORY_WRAPPER_REGISTRATOR_HPP__
#define __IWS_REACTOR_FACTORY_WRAPPER_REGISTRATOR_HPP__

#include <string>

#include "priorities.hpp"
#include "r.hpp"
#include "reactor.hpp"
#include "factory_wrapper.hpp"

namespace iws {
namespace reactor {

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

// ----

template<typename I, bool unregister>
factory_wrapper_registrator<I, unregister>::factory_wrapper_registrator(
      priorities priority, const typename factory_wrapper<I>::producer_function &producer)
      : _name(std::string())
      , _priority(priority)
{
   r.register_factory(_name, _priority, std::make_unique<factory_wrapper<I>>(producer));
}

template<typename I, bool unregister>
factory_wrapper_registrator<I, unregister>::factory_wrapper_registrator(
      const std::string &instance, priorities priority, const typename factory_wrapper<I>::producer_function &producer)
      : _name(instance)
      , _priority(priority)
{
   r.register_factory(_name, _priority, std::make_unique<factory_wrapper<I>>(producer));
}

template<typename I, bool unregister>
factory_wrapper_registrator<I, unregister>::~factory_wrapper_registrator()
{
   if (unregister)
   {
      r.unregister_factory(_name, _priority, typeid(I));
   }
}

} //namespace reactor
} // namespace iws

#endif //__IWS_REACTOR_FACTORY_WRAPPER_REGISTRATOR_HPP__
