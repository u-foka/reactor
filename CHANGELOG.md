Reactor Changelog
=================

#### Legend
- __[B]__ Breaking API change
- [D] New deprecated API

v2.3-next
--------
- respect cmake's BUILD_SHARED_LIBS option
- `callback_holder` improvements
  - Separate types to support forwarding rvalue reference arguments to a single callback or coying arguments to multiple arguments
  - __[B]__ Locking in `callback_holder` is now optional (default off now)
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