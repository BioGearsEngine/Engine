#!/bin/bash

rsync -av ../bin/config ../deploy/toolkit
rsync -av ../bin/ecg ../deploy/toolkit
rsync -av ../bin/environments ../deploy/toolkit
rsync -av ../bin/nutrition ../deploy/toolkit
rsync -av ../bin/patients ../deploy/toolkit
rsync -av ../bin/substances ../deploy/toolkit
rsync -av ../bin/xsd ../deploy/toolkit

rsync -av --exclude='Scenarios/*/Current Baseline/*' ../verification/Scenarios ../deploy/toolkit/

mkdir -p ../deploy/toolkit/release
mkdir -p ../deploy/toolkit/release64

cp ../bin/release/* ../deploy/toolkit/release
cp ../bin/release64/* ../deploy/toolkit/release64

cp ../bin/log4j.properties ../deploy/toolkit/
cp ../bin/*.jar ../deploy/toolkit/
cp ../lib/*.jar ../deploy/toolkit/
cp -r ../lib/jchart2d-3.2.2 ../deploy/toolkit/
cp ../bin/UCEDefs.txt ../deploy/toolkit/
cp ../bin/BioGearsConfiguration.xml ../deploy/toolkit
