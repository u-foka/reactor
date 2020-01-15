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

#include <reactor/r.hpp>

#include <reactor/reactor.hpp>

namespace iws::reactor {

// Memory for the reactor object
static typename std::aligned_storage<sizeof(reactor), alignof(reactor)>::type r_memory;
reactor &r = reinterpret_cast<reactor &>(r_memory);

// This is done before the dynamic initialization is started since
// std::atomic<int> has a trivial constructor and initialized with a constexpr
std::atomic<int> init::instance_count(0);

init::init()
{
   if (1 != ++instance_count)
      return;

   // Placement new for the global instance
   new (&r) reactor();
}

init::~init()
{
   if (0 != --instance_count)
      return;

   // Destruct global instance
   r.~reactor();
}

} // namespace iws::reactor
