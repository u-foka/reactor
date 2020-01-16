// Copyright 2020 Tamás Eisenberger <e.tamas@iwstudio.hu>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <reactor/contract_base.hpp>

#include <reactor/r.hpp>
#include <reactor/reactor.hpp>

namespace iws {
namespace reactor {

contract_base::contract_base()
      : _r_inst(&r)
{
   if (nullptr != _r_inst)
   {
      _r_inst->register_contract(this);
   }
}

contract_base::contract_base(reactor *r_inst)
      : _r_inst(r_inst)
{
   if (nullptr != _r_inst)
   {
      _r_inst->register_contract(this);
   }
}

contract_base::~contract_base()
{
   if (nullptr != _r_inst)
   {
      _r_inst->unregister_contract(this);
   }
}

} //namespace reactor
} // namespace iws
