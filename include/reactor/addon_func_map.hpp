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

#ifndef __IWS_REACTOR_ADDON_FUNC_MAP_HPP__
#define __IWS_REACTOR_ADDON_FUNC_MAP_HPP__

#include <map>
#include <type_traits>

#include "addon.hpp"
#include "priorities.hpp"

namespace iws {
namespace reactor {

template<typename T>
struct addon_func_map
{
   typedef std::multimap<priorities, typename std::add_pointer<addon<T>>::type> type;
};

} //namespace reactor
} // namespace iws

#endif //__IWS_REACTOR_ADDON_FUNC_MAP_HPP__
