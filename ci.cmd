call "%VCVARSALL%" x64
mkdir %BUILD_DIR%
cd %BUILD_DIR%
cmake ../ -DCMAKE_BUILD_TYPE=%BUILD_TYPE% # -DCODE_COVERAGE=ON # No code coverage on msvc yet
cmake --build . --target %TARGET%