call "%VCVARSALL%" x64
mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"
cmake ../ -DCMAKE_BUILD_TYPE="%BUILD_TYPE%" -DBUILD_SHARED_LIBS="%SHARED_LIBS%" -DCXX11_ENABLED="%CXX11%"
cmake --build . --target "%TARGET%"