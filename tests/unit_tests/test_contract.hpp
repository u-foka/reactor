#ifndef __IWS_REACTOR_TEST_CONTRACT_HPP__
#define __IWS_REACTOR_TEST_CONTRACT_HPP__

#include <reactor/index.hpp>

namespace iws {
namespace reactor_test {

template<typename T>
class test_contract : public iws::reactor::typed_contract<T>
{
   public:
   const iws::reactor::index _index;

   test_contract(const std::string &instance = std::string())
         : iws::reactor::typed_contract<T>(nullptr)
         , _index(typeid(T), instance)
   {
   }

   virtual const iws::reactor::index &get_index() const override { return _index; }
   virtual void try_get() override { throw std::logic_error("Not implemented"); }
};

} // namespace reactor_test
} // namespace iws

#endif // __IWS_REACTOR_TEST_CONTRACT_HPP__
