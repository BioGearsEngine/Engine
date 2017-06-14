del mingw /Q
mkdir mingw
cd mingw
mkdir debug
cd debug
cmake -Denv:STRING=mingw -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ../../../
cd ..
mkdir release
cd release
cmake -Denv:STRING=mingw -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ../../../
cd ..
cd ..