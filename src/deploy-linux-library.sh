#!/bin/bash

# bin
rm -rf ../library/bin
rm -rf ../library/include
rm -rf ../library/lib

mkdir ../library/bin
mkdir ../library/include
mkdir ../library/lib

cp -r ../bin/config ../library/bin
cp -r ../bin/patients ../library/bin
cp -r ../bin/environments ../library/bin
cp -r ../bin/nutrition ../library/bin
cp -r ../bin/ecg ../library/bin
cp -r ../bin/substances ../library/bin
cp -r ../bin/states ../library/bin
cp -r ../bin/xsd ../library/bin

cp ../bin/UCEDefs.txt ../library/bin
cp ../bin/log4j.properties ../library/bin
cp ../bin/BioGearsConfiguration.xml ../library/bin

# include
rsync -a --include '*' ../lib/Eigen-3.3.1/ ../library/include/Eigen-3.3.1/
rsync -a --include '*/' --include '*.h' --include '*.hh' --exclude '*' ../lib/log4cpp/ ../library/include/log4cpp
cp -r ../lib/xsd-4.0.0-x86_64-linux-gnu/libxsd/xsd ../library/include
cp -r ../lib/xerces-c-3.1.2/src/xercesc ../library/include
rsync -a --include '*/' --include '*.h' --include '*.hxx' --include='*.inl' --include='*.txt' --exclude '*' cdm/cpp/* ../library/include/cdm
rsync -a --include '*/' --include '*.h' --include '*.hxx' --include='*.inl' --include='*.txt' --exclude '*' schema/cpp/* ../library/include/schema
cp engine/cpp/BioGearsPhysiologyEngine.h ../library/include

# lib
cp ../lib/xerces-c-3.1.2/src/.libs/libxerces-c.a ../library/lib

folderAppend=
if [ -n "$1" ] && [ "$1" == "x64" ]; then
    folderAppend="64"
fi

cp ../bin/release$folderAppend/* ../library/lib
