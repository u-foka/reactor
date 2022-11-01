#include <gtest/gtest.h>

#include <reactor/client.hpp>

#include "i_test.hpp"
#include "test_contract.hpp"

using iws::reactor::r;
using iws::reactor_test::i_test;
using iws::reactor_test::test_contract;

namespace iws {
namespace reactor_test {

int init_test_helper()
{
   return r.get(test_contract<i_test>()).get_id();
}

} // namespace reactor_test
} // namespace iws
