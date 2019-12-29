#ifndef __IWS_REACTOR_INDEX_HPP__
#define __IWS_REACTOR_INDEX_HPP__

#include <string>
#include <typeindex>
#include <utility>

namespace iws::reactor {

/**
 * @brief Link to a reactor managed instance
 */
typedef std::pair<std::type_index, std::string> index;

} // namespace iws::reactor

#endif //__IWS_REACTOR_INDEX_HPP__
