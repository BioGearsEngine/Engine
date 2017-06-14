rm -rf raspberryPi
mkdir raspberryPi
cd raspberryPi
mkdir debug
cd debug
cmake -Denv=raspberryPi -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../../../
cd ..
mkdir release
cd release
cmake -Denv=raspberryPi -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ../../../
cd ../..
