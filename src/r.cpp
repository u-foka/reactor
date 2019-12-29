#include <reactor/r.hpp>

#include <reactor/reactor.hpp>

namespace iws::reactor {

// Memory for the reactor object
static typename std::aligned_storage<sizeof(reactor), alignof(reactor)>::type r_memory;
reactor &r = reinterpret_cast<reactor &>(r_memory);

// This is done before the dynamic initialization is started since
// std::atomic<int> has a trivial constructor and initialized with a constexpr
std::atomic<int> init::instance_count(0);

init::init()
{
   if (1 != ++instance_count)
      return;

   // Placement new for the global instance
   new (&r) reactor();
}

init::~init()
{
   if (0 != --instance_count)
      return;

   // Destruct global instance
   r.~reactor();
}

} // namespace iws::reactor
