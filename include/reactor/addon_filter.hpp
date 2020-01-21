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

#ifndef __IWS_REACTOR_ADDON_FILTER_HPP__
#define __IWS_REACTOR_ADDON_FILTER_HPP__

#include "addon_filter_base.hpp"

#include <functional>
#include <typeinfo>

#include "addon_func_map.hpp"

namespace iws {
namespace reactor {

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

// ----

template<typename T>
addon_filter<T>::addon_filter(func &&filter_func)
      : filter_func(filter_func)
{
}

template<typename T>
const std::type_info &addon_filter<T>::get_type() const
{
   return typeid(T);
}

template<typename T>
const std::type_info &addon_filter<T>::get_interface_type() const
{
   return typeid(typename T::interface);
}

} // namespace reactor
} // namespace iws

#endif //__IWS_REACTOR_ADDON_FILTER_HPP__
