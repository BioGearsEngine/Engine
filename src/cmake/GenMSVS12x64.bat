del msvs12x64 /Q
mkdir msvs12x64
cd msvs12x64
cmake -Denv:STRING=msvc -G "Visual Studio 12 2013 Win64" ../../
cd ..
xcopy /Y .\usr\msvs12x64\BioGearsScenarioDriver.vcxproj.user .\msvs12x64\engine\
xcopy /Y .\usr\msvs12x64\UnitTestDriver.vcxproj.user .\msvs12x64\test\