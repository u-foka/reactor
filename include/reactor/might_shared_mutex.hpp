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

#if __cplusplus < 201402L
#include <mutex>

namespace iws {
namespace detail {

class might_shared_mutex : public std::mutex
{
};

template<typename T>
class might_shared_lock : public std::unique_lock<T>
{
 public:
   might_shared_lock(T &mutex)
         : std::unique_lock<T>(mutex)
   {
   }
};

} // namespace detail
} // namespace iws

#else
#include <shared_mutex>

namespace iws {
namespace detail {

class might_shared_mutex : public std::shared_timed_mutex
{
};

template<typename T>
class might_shared_lock : public std::shared_lock<T>
{
 public:
   might_shared_lock(T &mutex)
         : std::shared_lock<T>(mutex)
   {
   }
};

} // namespace detail
} // namespace iws

#endif