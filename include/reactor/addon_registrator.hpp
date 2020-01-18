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

#ifndef __IWS_REACTOR_ADDON_REGISTRATOR_HPP__
#define __IWS_REACTOR_ADDON_REGISTRATOR_HPP__

#include "make_unique_polyfil.hpp"

namespace iws {
namespace reactor {

namespace pf = ::iws::polyfil;

template<typename T, bool unregister = false>
class addon_registrator
{
 public:
   addon_registrator(priorities priority, addon<T> &&inst);
   addon_registrator(const std::string &instance, priorities priority, addon<T> &&inst);

   ~addon_registrator();

 private:
   const std::string _name;
   const priorities _priority;
};

// ----

template<typename T, bool unregister>
addon_registrator<T, unregister>::addon_registrator(priorities priority, addon<T> &&inst)
      : _name(std::string())
      , _priority(priority)
{
   r.register_addon(_name, _priority, pf::make_unique<addon<T>>(std::move(inst)));
}

template<typename T, bool unregister>
addon_registrator<T, unregister>::addon_registrator(const std::string &instance, priorities priority, addon<T> &&inst)
      : _name(instance)
      , _priority(priority)
{
   r.register_addon(_name, _priority, pf::make_unique<addon<T>>(std::move(inst)));
}

template<typename T, bool unregister>
addon_registrator<T, unregister>::~addon_registrator()
{
   if (unregister)
   {
      r.unregister_addon(_name, _priority, typeid(T));
   }
}

} // namespace reactor
} // namespace iws

#endif //__IWS_REACTOR_ADDON_REGISTRATOR_HPP__
