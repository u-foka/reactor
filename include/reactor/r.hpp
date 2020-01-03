#ifndef __IWS_REACTOR_R_HPP__
#define __IWS_REACTOR_R_HPP__

#include <atomic>

#include <reactor/utils.hpp>

namespace iws::reactor {

/**
 * @brief Go AWAY! Don't touch!
 *
 * Nifty Counter
 * This class is used to initialize the global "r" object.
 */
class init
{
   public:
   init();
   ~init();

   private:
   static std::atomic<int> instance_count;
};

class reactor;
REACTOR_IMPORT extern reactor &r;
static const init __init_reactor;

} // namespace iws::reactor

#endif //__IWS_REACTOR_R_HPP__
