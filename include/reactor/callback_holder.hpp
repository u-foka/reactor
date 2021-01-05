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

#ifndef CALLBACK_HOLDER_HPP
#define CALLBACK_HOLDER_HPP

#include <functional>
#include <map>

#include "might_shared_mutex.hpp"
#include "utils.hpp"

namespace iws {

namespace pf = polyfil;

namespace detail {
namespace callback_holder {

enum callback_holder_arg_forward_mode
{
   CB_COPY_ARGS = 0,
   CB_FORWARD_ARGS
};

template<callback_holder_arg_forward_mode Mode>
struct call_trait
{
};

template<>
struct call_trait<CB_COPY_ARGS>
{
   struct type
   {
   };
};

template<>
struct call_trait<CB_FORWARD_ARGS>
{
   struct type
   {
   };
};

template<callback_holder_arg_forward_mode Mode>
using call_trait_t = typename call_trait<Mode>::type;

template<callback_holder_arg_forward_mode ArgForwardMode, typename Locks, typename... Args>
class callback_holder_impl
{
 public:
   typedef std::function<void(Args...)> value_type;

   callback_holder_impl()
         : _next_id(0)
         , _list()
         , _mutex()
   {
   }

   template<bool ENABLE = ArgForwardMode == CB_COPY_ARGS, reactor::detail::enable_if_t<ENABLE, int> = 0>
   size_t connect(value_type cb)
   {
      typename Locks::lock_unique lock(_mutex);

      _list[_next_id] = cb;
      return _next_id++;
   }

   template<bool ENABLE = ArgForwardMode == CB_COPY_ARGS, reactor::detail::enable_if_t<ENABLE, int> = 0>
   bool disconnect(size_t id)
   {
      typename Locks::lock_unique lock(_mutex);

      auto it = _list.find(id);
      if (it != _list.end())
      {
         _list.erase(it);
         return true;
      }

      return false;
   }

   template<bool ENABLE = ArgForwardMode == CB_FORWARD_ARGS, reactor::detail::enable_if_t<ENABLE, int> = 0>
   void set(value_type cb)
   {
      typename Locks::lock_unique lock(_mutex);

      _list[_next_id] = cb;
      // No stepping of _next_id here so we'll always override the existing callback in forwarding mode...
   }

   void clear()
   {
      typename Locks::lock_unique lock(_mutex);

      _list.clear();
   }

   void operator()(Args &&...args)
   {
      typename Locks::lock_shared lock(_mutex);

      call(call_trait_t<ArgForwardMode>(), std::forward<Args>(args)...);
   }

   size_t callback_count()
   {
      typename Locks::lock_shared lock(_mutex);

      return _list.size();
   }

   operator bool()
   {
      typename Locks::lock_shared lock(_mutex);

      return 0 != _list.size();
   }

 private:
   size_t _next_id;
   std::map<size_t, value_type> _list;
   typename Locks::mutex _mutex;

   void call(call_trait_t<CB_COPY_ARGS> /* dummy */, const Args &...args)
   {
      for (auto it : _list)
      {
         (it.second)(args...);
      }
   }

   void call(call_trait_t<CB_FORWARD_ARGS> /* dummy */, Args &&...args)
   {
      if (0 < _list.size())
      {
         _list.begin()->second(std::forward<Args>(args)...);
      }
   }
};

struct dummy_locks
{
   struct mutex
   {
   };

   struct lock_shared
   {
      lock_shared(mutex &) {}
   };

   struct lock_unique
   {
      lock_unique(mutex &) {}
   };
};

struct default_locks
{
   using mutex = pf::might_shared_mutex;
   using lock_shared = pf::might_shared_lock<mutex>;
   using lock_unique = std::unique_lock<mutex>;
};

} // namespace callback_holder
} // namespace detail

template<typename... Args>
using callback_holder = detail::callback_holder::callback_holder_impl<detail::callback_holder::CB_COPY_ARGS,
      detail::callback_holder::dummy_locks, Args...>;

template<typename... Args>
using forwarding_callback_holder =
      detail::callback_holder::callback_holder_impl<detail::callback_holder::CB_FORWARD_ARGS,
            detail::callback_holder::dummy_locks, Args...>;

template<typename... Args>
using threadsafe_callback_holder = detail::callback_holder::callback_holder_impl<detail::callback_holder::CB_COPY_ARGS,
      detail::callback_holder::default_locks, Args...>;

template<typename... Args>
using threadsafe_forwarding_callback_holder =
      detail::callback_holder::callback_holder_impl<detail::callback_holder::CB_FORWARD_ARGS,
            detail::callback_holder::default_locks, Args...>;

} // namespace iws

#endif // CALLBACK_HOLDER_HPP
