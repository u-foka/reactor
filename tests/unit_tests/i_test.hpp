#ifndef __IWS_REACTOR_I_TEST_HPP__
#define __IWS_REACTOR_I_TEST_HPP__

#include <string>
#include <functional>

namespace iws {
namespace reactor_test {

class i_test
{
   public:
   virtual ~i_test() {}
   struct test_addon
   {
      typedef i_test intf;
      typedef std::function<void(std::string)> func;
   };
   virtual int get_id() = 0;
};

} // namespace reactor_test
} // namespace iws

#endif // __IWS_REACTOR_I_TEST_HPP__
