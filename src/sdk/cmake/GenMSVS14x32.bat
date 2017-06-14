del msvs14x32 /Q
mkdir msvs14x32
cd msvs14x32
cmake -Denv:STRING=msvc -G "Visual Studio 14 2015" ../../
cd ..
xcopy /Y .\usr\msvs14x32\HowToDriver.vcxproj.user .\msvs14x32\