#ifndef __IWS_REACTOR_ADDON_FUNC_MAP_HPP__
#define __IWS_REACTOR_ADDON_FUNC_MAP_HPP__

#include <map>
#include <type_traits>

#include "addon.hpp"
#include "priorities.hpp"

namespace iws::reactor {

template<typename T>
struct addon_func_map
{
   typedef std::multimap<priorities, typename std::add_pointer<addon<T>>::type> type;
};

} // namespace iws::reactor

#endif //__IWS_REACTOR_ADDON_FUNC_MAP_HPP__
