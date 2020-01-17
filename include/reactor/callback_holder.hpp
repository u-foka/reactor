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

#ifndef CALLBACK_HOLDER_HPP
#define CALLBACK_HOLDER_HPP

#include <functional>
#include <map>

#include "might_shared_mutex.hpp"

namespace iws {

namespace pf = polyfil;

template<typename... Args>
class callback_holder
{
 public:
   typedef std::function<void(Args...)> value_type;

   callback_holder()
         : _next_id(0)
         , _list()
         , _mutex()
   {
   }

   size_t connect(value_type cb)
   {
      std::unique_lock<pf::might_shared_mutex> lock(_mutex);

      _list[_next_id] = cb;
      return _next_id++;
   }

   bool disconnect(size_t id)
   {
      std::unique_lock<pf::might_shared_mutex> lock(_mutex);

      auto it = _list.find(id);
      if (it != _list.end())
      {
         _list.erase(it);
         return true;
      }

      return false;
   }

   void clear()
   {
      std::unique_lock<pf::might_shared_mutex> lock(_mutex);
      _list.clear();
   }

   void operator()(Args &&... args)
   {
      pf::might_shared_lock<pf::might_shared_mutex> lock(_mutex);

      for (auto it : _list)
      {
         (it.second)(std::forward<Args>(args)...);
      }
   }

   size_t callback_count()
   {
      pf::might_shared_lock<pf::might_shared_mutex> lock(_mutex);
      return _list.size();
   }

   operator bool()
   {
      pf::might_shared_lock<pf::might_shared_mutex> lock(_mutex);
      return 0 != _list.size();
   }

 private:
   size_t _next_id;
   std::map<size_t, value_type> _list;
   pf::might_shared_mutex _mutex;
};

} // namespace iws

#endif // CALLBACK_HOLDER_HPP
