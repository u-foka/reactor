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

#ifndef __IWS_REACTOR_FACTORY_RESULT_HPP__
#define __IWS_REACTOR_FACTORY_RESULT_HPP__

#include <memory>
#include <typeindex>

namespace iws::reactor {

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

// ----

template<typename T>
factory_result::factory_result(std::shared_ptr<T> obj)
      : _obj(obj)
      , _id(typeid(T))
{
}

template<typename T>
std::shared_ptr<void> factory_result::get()
{
   if (_id != typeid(T))
   {
      throw std::logic_error("Factory returned bad type");
   }

   return _obj;
}

} // namespace iws::reactor

#endif // __IWS_REACTOR_FACTORY_RESULT_HPP__