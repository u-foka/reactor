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

#ifndef __IWS_MAKE_UNIQUE_POLYFIL__
#define __IWS_MAKE_UNIQUE_POLYFIL__

#if __cplusplus < 201402L // target < C++14

#include <memory>

namespace iws {
namespace polyfil {

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&... args)
{
   return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace polyfil
} // namespace iws

#else // target >= C++14

#include <memory>

namespace iws {
namespace polyfil {

using std::make_unique;

} // namespace polyfil
} // namespace iws

#endif // target <> C++14

#endif // __IWS_MAKE_UNIQUE_POLYFIL__