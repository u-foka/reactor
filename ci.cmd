call "%VCVARSALL%" x64
mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"
cmake ../ -DCMAKE_BUILD_TYPE="%BUILD_TYPE%"
cmake --build . --target "%TARGET%"