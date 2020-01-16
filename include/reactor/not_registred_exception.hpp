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

#ifndef __IWS_REACTOR_NOT_REGISTRED_EXCEPTION_HPP__
#define __IWS_REACTOR_NOT_REGISTRED_EXCEPTION_HPP__

#include <exception>
#include <sstream>
#include <string>

namespace iws {
namespace reactor {

class not_registred_exception : public std::exception
{
 public:
   not_registred_exception(const std::string &category, const std::type_info &type, const std::string &name);
   virtual const char *what() const noexcept;

 private:
   const std::string &_category;
   const std::type_info &_type;
   const std::string _name;
   mutable std::string _msg_buffer;
};

class factory_not_registred_exception : public not_registred_exception
{
 public:
   factory_not_registred_exception(const std::type_info &type, const std::string &name);

 private:
   static const std::string category;
};

class addon_not_registred_exception : public not_registred_exception
{
 public:
   addon_not_registred_exception(const std::type_info &type, const std::string &name);

 private:
   static const std::string category;
};

class addon_filter_not_registred_exception : public not_registred_exception
{
 public:
   addon_filter_not_registred_exception(const std::type_info &type, const std::string &name);

 private:
   static const std::string category;
};

} //namespace reactor
} // namespace iws

#endif //__IWS_REACTOR_NOT_REGISTRED_EXCEPTION_HPP__
