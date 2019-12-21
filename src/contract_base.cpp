#include <reactor/contract_base.hpp>

#include <reactor/r.hpp>
#include <reactor/reactor.hpp>

namespace iws::reactor {

contract_base::contract_base()
   : _r_inst(&r)
{
   _r_inst->register_contract(this);
}

contract_base::contract_base(reactor *r_inst)
   : _r_inst(r_inst)
{
   _r_inst->register_contract(this);
}

contract_base::~contract_base()
{
   _r_inst->unregister_contract(this);
}

} // namespace iws::reactor
