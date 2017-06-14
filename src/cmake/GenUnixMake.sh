rm -rf unixMake
mkdir unixMake
cd unixMake
mkdir debug
cd debug
cmake -Denv=unixMake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../../../
cd ..
mkdir release
cd release
cmake -Denv=unixMake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ../../../
cd ../..
