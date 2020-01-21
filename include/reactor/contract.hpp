// Copyright 2020 Tamas Eisenberger <e.tamas@iwstudio.hu>
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

#ifndef __IWS_REACTOR_CONTRACT_HPP__
#define __IWS_REACTOR_CONTRACT_HPP__

#include "typed_contract.hpp"

#include "index.hpp"
#include "reactor.hpp"

namespace iws {
namespace reactor {

template<typename T>
class contract : public typed_contract<T>
{
 public:
   contract(const std::string &instance = std::string());
   contract(reactor *r_inst, const std::string &instance = std::string());
   virtual ~contract();
   virtual const index &get_index() const override;
   virtual void try_get() override;

 private:
   const index _index;
};

// ----

template<typename T>
contract<T>::contract(const std::string &instance)
      : typed_contract<T>()
      , _index(typeid(T), instance)
{
}

template<typename T>
contract<T>::contract(reactor *r_inst, const std::string &instance)
      : typed_contract<T>(r_inst)
      , _index(typeid(T), instance)
{
}

template<typename T>
contract<T>::~contract()
{
}

template<typename T>
const index &contract<T>::get_index() const
{
   return _index;
}

template<typename T>
void contract<T>::try_get()
{
   contract_base::_r_inst->get(*this);
}

} // namespace reactor
} // namespace iws

#endif //__IWS_REACTOR_CONTRACT_HPP__
