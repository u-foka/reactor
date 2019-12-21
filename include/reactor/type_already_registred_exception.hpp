#ifndef __IWS_REACTOR_TYPE_ALREADY_REGISTRED_EXCEPTION_HPP__
#define __IWS_REACTOR_TYPE_ALREADY_REGISTRED_EXCEPTION_HPP__

#include <exception>
#include <typeinfo>
#include <string>

#include "priorities.hpp"

namespace iws::reactor {

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

} // namespace iws::reactor

#endif //__IWS_REACTOR_TYPE_ALREADY_REGISTRED_EXCEPTION_HPP__
