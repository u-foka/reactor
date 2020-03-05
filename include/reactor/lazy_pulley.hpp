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

#ifndef __IWS_REACTOR_LAZY_PULLEY_HPP__
#define __IWS_REACTOR_LAZY_PULLEY_HPP__

#include "contract.hpp"
#include "r.hpp"
#include "reactor.hpp"

namespace iws {
namespace reactor {

template<typename T>
class lazy_pulley
{
 public:
   static const contract<T> _contract;

   lazy_pulley();

   T *operator->() const;

 private:
   mutable T *_obj;
};

// ----

template<typename T>
const contract<T> lazy_pulley<T>::_contract;

template<typename T>
lazy_pulley<T>::lazy_pulley()
      : _obj(nullptr)
{
}

template<typename T>
T *lazy_pulley<T>::operator->() const
{
   // No locing or anything here, r.get() is already thread safe, wors case we'll get the same object twice and store it twice.
   // (storage of the pointer should be atomic as pointer size matches instruction size)
   if (nullptr == _obj)
   {
      _obj = &r.get(_contract);
   }
   
   return _obj;
}

} // namespace reactor
} // namespace iws

#endif // __IWS_REACTOR_LAZY_PULLEY_HPP__