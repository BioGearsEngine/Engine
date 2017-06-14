del msvs12x32 /Q
mkdir msvs12x32
cd msvs12x32
cmake -Denv:STRING=msvc -G "Visual Studio 12 2013" ../../
cd ..
xcopy /Y .\usr\msvs12x32\BioGearsScenarioDriver.vcxproj.user .\msvs12x32\engine\
xcopy /Y .\usr\msvs12x32\UnitTestDriver.vcxproj.user .\msvs12x32\test\