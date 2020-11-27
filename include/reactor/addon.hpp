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

#ifndef __IWS_REACTOR_ADDON_HPP__
#define __IWS_REACTOR_ADDON_HPP__

#include "addon_base.hpp"

namespace iws {
namespace reactor {

template<typename T>
class addon : public addon_base
{
 public:
   addon(typename T::func &&addon_func);
   addon(const typename T::func &addon_func);

   const typename T::func addon_func;
   virtual const std::type_info &get_type() const override;
   virtual const std::type_info &get_interface_type() const override;
};

// ----

template<typename T>
addon<T>::addon(typename T::func &&addon_func)
      : addon_func(addon_func)
{
}

template<typename T>
addon<T>::addon(const typename T::func &addon_func)
      : addon_func(addon_func)
{
}

template<typename T>
const std::type_info &addon<T>::get_type() const
{
   return typeid(T);
}

template<typename T>
const std::type_info &addon<T>::get_interface_type() const
{
   return typeid(typename T::interface);
}

} // namespace reactor
} // namespace iws

#endif //__IWS_REACTOR_ADDON_HPP__
