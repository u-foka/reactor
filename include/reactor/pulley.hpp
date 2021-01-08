// Copyright 2021 Tamas Eisenberger <e.tamas@iwstudio.hu>
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

#include <memory>

#include "pulley_contract.hpp"
#include "r.hpp"
#include "reactor.hpp"

namespace iws {
namespace reactor {

enum pulley_type
{
   reference_pulley = 0,
   lazy_reference_pulley,
   shared_ptr_pulley
};

template<typename T, pulley_type type, typename enable = void>
class pulley_base;

template<typename T, pulley_type type>
class pulley_base<T, type, detail::enable_if_t<reference_pulley == type>>
{
 public:
   T *get() const;

 protected:
   pulley_base() = delete;
   explicit pulley_base(const typed_contract<T> &contract);

 private:
   const typed_contract<T> &_contract;
   T &_obj;
};

template<typename T, pulley_type type>
class pulley_base<T, type, detail::enable_if_t<lazy_reference_pulley == type>>
{
 public:
   T *get() const;

 protected:
   pulley_base() = delete;
   explicit pulley_base(const typed_contract<T> &contract);

 private:
   const typed_contract<T> &_contract;
   mutable T *_obj;
};

template<typename T, pulley_type type>
class pulley_base<T, type, detail::enable_if_t<shared_ptr_pulley == type>>
{
 public:
   T *get() const;

 protected:
   pulley_base() = delete;
   explicit pulley_base(const typed_contract<T> &contract);

 private:
   const typed_contract<T> &_contract;
   std::shared_ptr<T> _obj;
};

template<typename T, pulley_type type = reference_pulley>
class pulley : public pulley_base<T, type>
{
 public:
   static const pulley_contract<T> _contract;

   pulley();

   T *operator->() const;
};

// ----

template<typename T, pulley_type type>
pulley_base<T, type, detail::enable_if_t<reference_pulley == type>>::pulley_base(const typed_contract<T> &contract)
      : _contract(contract)
      , _obj(r.get(_contract))
{
}

template<typename T, pulley_type type>
T *pulley_base<T, type, detail::enable_if_t<reference_pulley == type>>::get() const
{
   return &_obj;
}

template<typename T, pulley_type type>
pulley_base<T, type, detail::enable_if_t<lazy_reference_pulley == type>>::pulley_base(const typed_contract<T> &contract)
      : _contract(contract)
      , _obj(nullptr)
{
}

template<typename T, pulley_type type>
T *pulley_base<T, type, detail::enable_if_t<lazy_reference_pulley == type>>::get() const
{
   // No locking or anything here, r.get() is already thread safe, worst case we'll get the same object twice and store
   // it twice.
   // (storage of the pointer should be atomic as pointer size matches instruction size)
   if (nullptr == _obj)
   {
      _obj = &r.get(_contract);
   }
   return _obj;
}

template<typename T, pulley_type type>
pulley_base<T, type, detail::enable_if_t<shared_ptr_pulley == type>>::pulley_base(const typed_contract<T> &contract)
      : _contract(contract)
      , _obj(r.get_ptr(r.get(_contract)))
{
}

template<typename T, pulley_type type>
T *pulley_base<T, type, detail::enable_if_t<shared_ptr_pulley == type>>::get() const
{
   return _obj.get();
}

template<typename T, pulley_type type>
const pulley_contract<T> pulley<T, type>::_contract;

template<typename T, pulley_type type>
pulley<T, type>::pulley()
      : pulley_base<T, type>(_contract.get())
{
}

template<typename T, pulley_type type>
T *pulley<T, type>::operator->() const
{
   return pulley_base<T, type>::get();
}

} // namespace reactor
} // namespace iws

#endif // __IWS_REACTOR_PULLEY_HPP__