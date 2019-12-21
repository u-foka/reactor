#ifndef __IWS_REACTOR_ADDON_BASE_HPP__
#define __IWS_REACTOR_ADDON_BASE_HPP__

#include <typeinfo>

namespace iws::reactor {

class addon_base
{
 public:
   virtual ~addon_base() {}
   virtual const std::type_info &get_type() const = 0;
   virtual const std::type_info &get_interface_type() const = 0;
};

} // namespace iws::reactor

#endif //__IWS_REACTOR_ADDON_BASE_HPP__
