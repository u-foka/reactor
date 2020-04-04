#include "i_ext_test.hpp"

#include <reactor/provider.hpp>

namespace iws {
namespace reactor_test {

class ext_test_impl : public i_ext_test
{
 public:
   ext_test_impl() {}
   virtual bool are_you_ext() { return true; }
};

static const reactor::factory_registrator<i_ext_test, ext_test_impl> registrator(reactor::prio_normal);

} // namespace reactor_test
} // namespace iws
