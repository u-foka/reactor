// Copyright 2021 Tamas Eisenberger <e.tamas@iwstudio.hu>
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

#ifndef __IWS_REACTOR_FACTORY_WRAPPER_HPP__
#define __IWS_REACTOR_FACTORY_WRAPPER_HPP__

#include "factory_base.hpp"

#include <memory>

namespace iws {
namespace reactor {

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
   virtual factory_result produce(const std::string &instance) const override;

 private:
   producer_function _producer;
};

// ----

template<typename I>
factory_wrapper<I>::factory_wrapper(const producer_function &producer)
      : factory_base(typeid(I))
      , _producer(producer)
{
}

template<typename I>
factory_result factory_wrapper<I>::produce(const std::string &instance) const
{
   return _producer(instance);
}

} // namespace reactor
} // namespace iws

#endif //__IWS_REACTOR_FACTORY_WRAPPER_HPP__
