#ifndef __IWS_REACTOR_I_EXT_TEST_HPP__
#define __IWS_REACTOR_I_EXT_TEST_HPP__

namespace iws {
namespace reactor_test {

class i_ext_test
{
 public:
   virtual ~i_ext_test() {}
   virtual bool are_you_ext() = 0;
};

} // namespace reactor_test
} // namespace iws

#endif // __IWS_REACTOR_I_EXT_TEST_HPP__
