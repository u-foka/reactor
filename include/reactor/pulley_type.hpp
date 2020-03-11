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

#ifndef __IWS_REACTOR_PULLEY_TYPE_HPP__
#define __IWS_REACTOR_PULLEY_TYPE_HPP__

namespace iws {
namespace reactor {

enum pulley_type
{
	reference_pulley = 0,
	shared_ptr_pulley
};

} // namespace reactor
} // namespace iws

#endif // __IWS_REACTOR_PULLEY_TYPE_HPP__