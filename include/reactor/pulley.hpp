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

#ifndef __IWS_REACTOR_PULLEY_HPP__
#define __IWS_REACTOR_PULLEY_HPP__

#include "contract.hpp"
#include "r.hpp"
#include "reactor.hpp"

namespace iws {
namespace reactor {

template<typename T>
class pulley
{
 public:
   pulley();

   T *operator->();

 private:
   static contract<T> _contract;

   T &_obj;
};

// ----

template<typename T>
contract<T> pulley<T>::_contract;

template<typename T>
pulley<T>::pulley()
      : _obj(r.get(_contract))
{
}

template<typename T>
T *pulley<T>::operator->()
{
   return &_obj;
}

} // namespace reactor
} // namespace iws

#endif // __IWS_REACTOR_PULLEY_HPP__