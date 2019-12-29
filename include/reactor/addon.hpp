#ifndef __IWS_REACTOR_ADDON_HPP__
#define __IWS_REACTOR_ADDON_HPP__

#include "addon_base.hpp"

namespace iws::reactor {

template<typename T>
class addon : public addon_base
{
 public:
   addon(typename T::func &&addon_func);

   const typename T::func addon_func;
   virtual const std::type_info &get_type() const override;
   virtual const std::type_info &get_interface_type() const override;
};

// ----

template<typename T>
addon<T>::addon(typename T::func &&addon_func)
      : addon_func(addon_func)
{
}

template<typename T>
const std::type_info &addon<T>::get_type() const
{
   return typeid(T);
}

template<typename T>
const std::type_info &addon<T>::get_interface_type() const
{
   return typeid(typename T::interface);
}

} // namespace iws::reactor

#endif //__IWS_REACTOR_ADDON_HPP__
