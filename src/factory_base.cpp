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

#include <reactor/factory_base.hpp>

namespace iws::reactor {

factory_base::factory_base(const std::type_info &type)
      : _type(type)
{
}

factory_base::~factory_base() {}

const std::type_info &factory_base::get_type() const
{
   return _type;
}

} // namespace iws::reactor
