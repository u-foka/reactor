#ifndef __IWS_REACTOR_TYPED_CONTRACT_HPP__
#define __IWS_REACTOR_TYPED_CONTRACT_HPP__

#include "contract_base.hpp"

namespace iws::reactor {

template<typename T>
class typed_contract : public contract_base
{
 public:
   typed_contract();
   explicit typed_contract(reactor *r_inst);
};

template<typename T>
typed_contract<T>::typed_contract()
      : contract_base()
{
}

template<typename T>
typed_contract<T>::typed_contract(reactor *r_inst)
      : contract_base(r_inst)
{
}

} // namespace iws::reactor

#endif //__IWS_REACTOR_TYPED_CONTRACT_HPP__
