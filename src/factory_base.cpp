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
