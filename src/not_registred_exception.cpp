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

#include <reactor/not_registred_exception.hpp>

namespace iws {
namespace reactor {

not_registred_exception::not_registred_exception(
      const std::string &category, const std::type_info &type, const std::string &name)
      : _category(category)
      , _type(type)
      , _name(name)
{
}

const char *not_registred_exception::what() const noexcept
{
   std::stringstream ss;
   ss << "There is no " << _category << " with the given type or name registred: " << _type.name() << ", '" << _name
      << "'";
   _msg_buffer = ss.str();

   return _msg_buffer.c_str();
}

const std::string factory_not_registred_exception::category("factory");
factory_not_registred_exception::factory_not_registred_exception(const std::type_info &type, const std::string &name)
      : not_registred_exception(category, type, name)
{
}

const std::string addon_not_registred_exception::category("addon");
addon_not_registred_exception::addon_not_registred_exception(const std::type_info &type, const std::string &name)
      : not_registred_exception(category, type, name)
{
}

const std::string addon_filter_not_registred_exception::category("addon filter");
addon_filter_not_registred_exception::addon_filter_not_registred_exception(
      const std::type_info &type, const std::string &name)
      : not_registred_exception(category, type, name)
{
}

} // namespace reactor
} // namespace iws
