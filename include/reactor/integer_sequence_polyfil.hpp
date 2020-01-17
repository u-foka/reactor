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

#ifndef __IWS_INTEGER_SEQUENCE_POLYFIL_HPP__
#define __IWS_INTEGER_SEQUENCE_POLYFIL_HPP__

#if __cplusplus < 201402L // target < C++14
#include <cstddef>

namespace iws {
namespace polyfil {

template<typename T, T... Ints>
struct integer_sequence
{
   typedef T value_type;
   static constexpr std::size_t size() { return sizeof...(Ints); }
};

// @cond Doxygen_Ignore
template<typename T, std::size_t N, T... Is>
struct make_integer_sequence : make_integer_sequence<T, N - 1, N - 1, Is...>
{
};
// @endcond Doxygen_Ignore

template<typename T, T... Is>
struct make_integer_sequence<T, 0, Is...> : integer_sequence<T, Is...>
{
};

template<std::size_t... Ints>
using index_sequence = integer_sequence<std::size_t, Ints...>;

template<std::size_t N>
using make_index_sequence = make_integer_sequence<std::size_t, N>;

template<typename... T>
using index_sequence_for = make_index_sequence<sizeof...(T)>;

} // namespace polyfil
} // namespace iws

#else // target >= C++14
#include <utility>

namespace iws {
namespace polyfil {

using std::integer_sequence;
using std::make_integer_sequence;
using std::index_sequence;
using std::make_index_sequence;
using std::index_sequence_for;

} // namespace polyfil
} // namespace iws

#endif // target <> C++14

#endif // __IWS_INTEGER_SEQUENCE_POLYFIL_HPP__