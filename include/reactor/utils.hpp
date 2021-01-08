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

#ifndef REACTOR_UTILS_HPP
#define REACTOR_UTILS_HPP

#include <algorithm>
#include <type_traits>

/* Unused variable helper */
#define UNUSED(X) (void)(X)

/* Stringify helper */
#define X_MACRO_STR(s) #s
#define MACRO_STR(s) X_MACRO_STR(s)

namespace iws {
namespace reactor {
namespace detail {

template<typename Container>
typename Container::iterator find(Container &container, const typename Container::value_type &what)
{
   return std::find(container.begin(), container.end(), what);
}

template<typename Container>
typename Container::const_iterator find(const Container &container, const typename Container::value_type &what)
{
   return std::find(container.cbegin(), container.cend(), what);
}

template<typename Container, typename UnaryPredicate>
typename Container::iterator find_if(Container &container, const UnaryPredicate &pred)
{
   return std::find_if(container.begin(), container.end(), pred);
}

template<typename Container, typename UnaryPredicate>
typename Container::const_iterator find_if(const Container &container, const UnaryPredicate &pred)
{
   return std::find_if(container.cbegin(), container.cend(), pred);
}

template<typename ContainerT, typename PredicateT>
void erase_if(ContainerT &items, const PredicateT &predicate)
{
   for (auto it = items.begin(); it != items.end();)
   {
      if (predicate(*it))
         it = items.erase(it);
      else
         ++it;
   }
}

// C++11 lacks this and it's easier to use the replacement everywhere than switching between library implementation and
// polyfil..
template<bool _Test, class _Ty = void>
using enable_if_t = typename std::enable_if<_Test, _Ty>::type;

} // namespace detail
} // namespace reactor
} // namespace iws

#endif // REACTOR_UTILS_HPP