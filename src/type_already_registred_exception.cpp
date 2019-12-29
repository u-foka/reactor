#include <reactor/type_already_registred_exception.hpp>

#include <sstream>

namespace iws::reactor {

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

} // namespace iws::reactor
