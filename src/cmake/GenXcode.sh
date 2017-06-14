rm -rf xcode
mkdir xcode
cd xcode
mkdir debug
cd debug
cmake -Denv=xcode -G "Xcode" -DCMAKE_BUILD_TYPE=Debug ../../../
cd ..
mkdir release
cd release
cmake -Denv=xcode -G "Xcode" -DCMAKE_BUILD_TYPE=Release ../../../
cd ../..
