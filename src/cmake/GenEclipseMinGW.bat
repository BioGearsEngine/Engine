del eclipsemingw /Q
mkdir eclipsemingw
cd eclipsemingw
mkdir debug
cd debug
cmake -Denv:STRING=eclipsemingw -G "Eclipse CDT4 - MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ../../../
cd ..
mkdir release
cd release
cmake -Denv:STRING=eclipsemingw -G "Eclipse CDT4 - MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ../../../
cd ..
cd ..