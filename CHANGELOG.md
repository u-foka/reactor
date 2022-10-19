Reactor Changelog
=================

#### Legend
- __[B]__ Breaking API change
- [F] Bufgix
- [D] New deprecated API

v2.5-next
-----------
__[F]__ Fix build on newer clang
__[F]__ Fix building shared library on windows

v2.5
-----------
- __[B]__ In addons, the required `interface` member is renamed to `intf` ang `get_interface_type()` functions are
  renamed to `get_intf_type()`. This change was necessary to resolve a conflict with a macro `interface` defined in
  `combaseapi.h` from Microsoft. Thanks M$... very well done :(

v2.4.1
------
- [F] Fix pulley contract potential crash. The contract in pulley now makes sure the global r is initialized before the 
  contract tries to register itself.

v2.4
----
- __[B]__ respect cmake's BUILD_SHARED_LIBS option
- A pulley::get() is now public, providing access to the raw pointer of the stored object
- Addons can now be also created by copying a functor object (instead of move only)
- `callback_holder` improvements
  - Separate types to support forwarding rvalue reference arguments to a single callback or coying arguments to multiple
    arguments
  - __[B]__ Locking in `callback_holder` is now optional (default off now)
- [F] Added missing include <stdexcept> in factory_result.hpp
- __[B]__  Minimum required cmake version is now 3.1
- Introduced `CHANGELOG.md`

v2.3
----
- Support for `C++17` compilers
- Create packaged versions of releases in CI
  - Support for `VERSION` file when packaged
- 

v2.2
----
- New `pulley` types introduced
  - `reference_pulley` (the old behavior)
  - `lazy_reference_pulley`
  - `shared_ptr_pulley`
- `pulley` works now in const context
- It's now possible to query if an object has already been created or not (`bool instance_exists(&contract)`)
- Doxygen docs extended and built withint the CI
- Improved version detection from git tags

v2.1
----
- Windows support added
- `pulley` introduced
- Circular dependency detection
- Added support for `C++11` compilers (the default is still `C++14`)
- Added `README.md`
- __[B]__ `prio_unittest` -> `prio_test`
- __[B]__ Google Test is now pulled through a submodule

v2.0
----
- __[B]__ Move into dedicated namespace
- Added convinience headers
- __[B]__ Separate and optional global `r` instance

v1.0
----
Initial release