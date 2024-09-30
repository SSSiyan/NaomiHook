mkdir build64
cd build64
REM CLANG LINE
REM cmake .. -G "Visual Studio 16 2019" -A x64 -T ClangCl -DDEVELOPER_MODE=OFF
REM ICL LINE
REM cmake .. -G "Visual Studio 16 2019" -A x64 -T "Intel C++ Compiler 19.0" -DDEVELOPER_MODE=OFF
cmake .. -G "Visual Studio 16 2019" -A x64 -T ClangCl -DDEVELOPER_MODE=OFF
cmake --build . --config Release
cd ..
pause