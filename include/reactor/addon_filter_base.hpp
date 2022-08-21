// Copyright 2022 Tamas Eisenberger <e.tamas@iwstudio.hu>
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

#ifndef __IWS_REACTOR_ADDON_FILTER_BASE_HPP__
#define __IWS_REACTOR_ADDON_FILTER_BASE_HPP__

#include <typeinfo>

namespace iws {
namespace reactor {

class addon_filter_base
{
 public:
   virtual ~addon_filter_base() {}
   virtual const std::type_info &get_type() const = 0;
   virtual const std::type_info &get_intf_type() const = 0;
};

} // namespace reactor
} // namespace iws

#endif //__IWS_REACTOR_ADDON_FILTER_BASE_HPP__
