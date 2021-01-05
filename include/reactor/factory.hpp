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

#ifndef __IWS_REACTOR_FACTORY_HPP__
#define __IWS_REACTOR_FACTORY_HPP__

#include "factory_base.hpp"

#include "integer_sequence_polyfil.hpp"
#include "utils.hpp"

namespace iws {
namespace reactor {

namespace pf = ::iws::polyfil;

/**
 * @brief default factory implementation
 *
 * This factory can construct any type, and return it downcasted as it's base class (preferably interface).
 * It holds the given constructor arguments, and passes them to the constructor of the given type.
 * It can also pass an instance name to the constructor as the first parameter.
 *
 * @tparam I The returned type (preferably an interface class).
 * @tparam T The constructed type.
 * @tparam pass_name If true, the constructor of T gets the instance name as it's first argument.
 * @tparam Args the types of constructor argumens.
 */
template<typename I, typename T, bool pass_name, typename... Args>
class factory : public factory_base
{
 public:
   /**
    * @brief factory constructor
    * @param args arguments to be passed to the constructor of T.
    */
   factory(Args &&...args);
   /**
    * @brief produce a new object
    * @param instance is the name of the instance constructed
    *        (passed to the constructor of T if pass_name is true).
    * @return returns an shared_ptr holding an instance of T downcasted to I.
    */
   virtual factory_result produce(const std::string &instance) const override;

 private:
   std::tuple<Args...> _args;

   template<bool do_pass_name, size_t... Idx, detail::enable_if_t<!do_pass_name, int> = 0>
   factory_result produce_impl(const std::string &, pf::index_sequence<Idx...>) const;

   template<bool do_pass_name, size_t... Idx, detail::enable_if_t<do_pass_name, int> = 0>
   factory_result produce_impl(const std::string &instance, pf::index_sequence<Idx...>) const;
};

// ----

template<typename I, typename T, bool pass_name, typename... Args>
factory<I, T, pass_name, Args...>::factory(Args &&...args)
      : factory_base(typeid(I))
      , _args(std::forward<Args>(args)...)
{
}

template<typename I, typename T, bool pass_name, typename... Args>
factory_result factory<I, T, pass_name, Args...>::produce(const std::string &instance) const
{
   return produce_impl<pass_name>(instance, pf::index_sequence_for<Args...>());
}

template<typename I, typename T, bool pass_name, typename... Args>
template<bool do_pass_name, size_t... Idx, detail::enable_if_t<!do_pass_name, int>>
factory_result factory<I, T, pass_name, Args...>::produce_impl(const std::string &, pf::index_sequence<Idx...>) const
{
   return std::shared_ptr<I>(std::make_shared<T>(std::get<Idx>(_args)...));
}

template<typename I, typename T, bool pass_name, typename... Args>
template<bool do_pass_name, size_t... Idx, detail::enable_if_t<do_pass_name, int>>
factory_result factory<I, T, pass_name, Args...>::produce_impl(
      const std::string &instance, pf::index_sequence<Idx...>) const
{
   return std::shared_ptr<I>(std::make_shared<T>(instance, std::get<Idx>(_args)...));
}

} // namespace reactor
} // namespace iws

#endif //__IWS_REACTOR_FACTORY_HPP__
