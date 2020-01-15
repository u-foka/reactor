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
