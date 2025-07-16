mkdir build86_shipping
cd build86_shipping
cmake .. -G "Visual Studio 17 2022" -A "Win32" -DBUILD_ASI:BOOL=ON
cmake --build . --config Release
cd ..
PAUSE