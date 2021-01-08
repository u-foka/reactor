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

#ifndef __IWS_REACTOR_TYPE_ALREADY_REGISTRED_EXCEPTION_HPP__
#define __IWS_REACTOR_TYPE_ALREADY_REGISTRED_EXCEPTION_HPP__

#include <exception>
#include <string>
#include <typeinfo>

#include "priorities.hpp"

namespace iws {
namespace reactor {

class type_already_registred_exception : public std::exception
{
 public:
   type_already_registred_exception(const std::type_info &type, const std::string &name, priorities priority);
   virtual const char *what() const noexcept;

 private:
   const std::type_info &type;
   const std::string name;
   const priorities priority;
   mutable std::string msg_buffer;
};

} // namespace reactor
} // namespace iws

#endif //__IWS_REACTOR_TYPE_ALREADY_REGISTRED_EXCEPTION_HPP__
