#ifndef __IWS_REACTOR_ADDON_FILTER_HPP__
#define __IWS_REACTOR_ADDON_FILTER_HPP__

#include "addon_filter_base.hpp"

#include <functional>
#include <typeinfo>

#include "addon_func_map.hpp"

namespace iws::reactor {

template<typename T>
class addon_filter : public addon_filter_base
{
   public:
   typedef std::function<void(typename addon_func_map<T>::type &)> func;

   addon_filter(func &&filter_func);

   const func filter_func;
   virtual const std::type_info &get_type() const override;
   virtual const std::type_info &get_interface_type() const override;
};

// ----

template<typename T>
addon_filter<T>::addon_filter(func &&filter_func)
      : filter_func(filter_func)
{
}

template<typename T>
const std::type_info &addon_filter<T>::get_type() const
{
   return typeid(T);
}

template<typename T>
const std::type_info &addon_filter<T>::get_interface_type() const
{
   return typeid(typename T::interface);
}

} // namespace iws::reactor

#endif //__IWS_REACTOR_ADDON_FILTER_HPP__
