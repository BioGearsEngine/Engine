del msvs14x64 /Q
mkdir msvs14x64
cd msvs14x64
cmake -Denv:STRING=msvc -G "Visual Studio 14 2015 Win64" ../../
cd ..
xcopy /Y .\usr\msvs14x64\BioGearsScenarioDriver.vcxproj.user .\msvs14x64\engine\
xcopy /Y .\usr\msvs14x64\UnitTestDriver.vcxproj.user .\msvs14x64\test\