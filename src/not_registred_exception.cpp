#include <reactor/not_registred_exception.hpp>

namespace iws::reactor {

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

} // namespace iws::reactor
