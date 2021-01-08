call "%VCVARSALL%" x64 || exit /b
mkdir "%BUILD_DIR%" || exit /b
cd "%BUILD_DIR%" || exit /b
cmake ../ -DCMAKE_BUILD_TYPE="%BUILD_TYPE%" -DBUILD_SHARED_LIBS="%SHARED_LIBS%" -DCXX11_ENABLED="%CXX11%" || exit /b
cmake --build . --target "%TARGET%" || exit /b