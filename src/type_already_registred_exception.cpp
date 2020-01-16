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

#include <reactor/type_already_registred_exception.hpp>

#include <sstream>

namespace iws {
namespace reactor {

reactor::type_already_registred_exception::type_already_registred_exception(
      const std::type_info &type, const std::string &name, priorities priority)
      : type(type)
      , name(name)
      , priority(priority)
{
}

const char *reactor::type_already_registred_exception::what() const noexcept
{
   std::stringstream ss;
   ss << "There is already a factory with the given type, name and priority registred: " << type.name() << ", '" << name
      << "', " << priority;
   msg_buffer = ss.str();

   return msg_buffer.c_str();
}

} //namespace reactor
} // namespace iws
