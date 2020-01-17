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

#ifndef __IWS_REACTOR_FACTORY_BASE_HPP__
#define __IWS_REACTOR_FACTORY_BASE_HPP__

#include <string>
#include <typeinfo>

#include "factory_result.hpp"

namespace iws {
namespace reactor {

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

} //namespace reactor
} // namespace iws

#endif //__IWS_REACTOR_FACTORY_BASE_HPP__
