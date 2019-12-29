#ifndef __IWS_REACTOR_PRIORITIES_HPP__
#define __IWS_REACTOR_PRIORITIES_HPP__

namespace iws::reactor {

/**
 * @brief Enumeration holding possible priority levels
 */
enum priorities
{
   prio_fallback = 100,
   prio_normal = 200,
   prio_override = 300,
   prio_unittest = 400,
};

} // namespace iws::reactor

#endif // __IWS_REACTOR_PRIORITIES_HPP__
