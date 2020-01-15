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