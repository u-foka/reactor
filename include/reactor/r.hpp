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

#ifndef __IWS_REACTOR_R_HPP__
#define __IWS_REACTOR_R_HPP__

#include <atomic>

#include <reactor/reactor_dll.hpp>

namespace iws {
namespace reactor {

/**
 * @brief Go AWAY! Don't touch!
 *
 * Nifty Counter
 * This class is used to initialize the global "r" object.
 */
class init
{
 public:
   init();
   ~init();

 private:
   static std::atomic<int> instance_count;
};

class reactor;
REACTOR_IMPORT extern reactor &r;
static const init __init_reactor;

} // namespace reactor
} // namespace iws

#endif //__IWS_REACTOR_R_HPP__
