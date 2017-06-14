mkdir ../deploy/temp

cp ../deploy/toolkit/BioGearsGUI.bat ../deploy/temp/
cp ../deploy/toolkit/BioGearsGUI.sh ../deploy/temp/
cp ../deploy/toolkit/CompareResults.bat ../deploy/temp/
cp ../deploy/toolkit/GraphResults.bat ../deploy/temp/
cp ../deploy/toolkit/msvcp120.dll ../deploy/temp/
cp ../deploy/toolkit/msvcr120.dll ../deploy/temp/
cp ../deploy/toolkit/vccorlib120.dll ../deploy/temp/
cp ../deploy/toolkit/appbundler-1.0.jar ../deploy/temp/
cp ../deploy/toolkit/build.xml ../deploy/temp/
cp ../deploy/toolkit/postbundle.sh ../deploy/temp/

rm -r ../deploy/toolkit/*
cp ../deploy/temp/* ../deploy/toolkit/
rm -r ../deploy/temp
