#ifndef __IWS_REACTOR_CONTRACT_BASE_HPP__
#define __IWS_REACTOR_CONTRACT_BASE_HPP__

#include "index.hpp"

namespace iws::reactor {

class reactor;

class contract_base
{
 public:
   contract_base();
   explicit contract_base(reactor *r_inst);
   virtual ~contract_base();
   virtual const index &get_index() const = 0;

 private:
   reactor *_r_inst;
};

} // namespace iws::reactor

#endif //__IWS_REACTOR_CONTRACT_BASE_HPP__
