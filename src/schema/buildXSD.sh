#!/bin/bash

if [[ $# != 1 ]]
then
    echo "Error, expected usage: buildXSD.sh [osx|linux]"
    exit 1
fi

cd cpp
mkdir -p bind
cd bind
rm -f *.hxx
rm -f *.cxx
cp ../custom-double/*.hxx .

# File per type
cp ../custom-double/biogears-cdm.cxx .

if [[ $1 = "osx" ]]
then
# removed --export-symbol __declspec(dllexport) from the following two lines:
../../../../lib/xsd-4.0.0-i686-macosx/bin/xsd cxx-tree --std c++11 --file-per-type --extern-xml-schema data-model-schema.xsd --cxx-prologue "#include \"biogears-cdm.hxx\"" --generate-polymorphic --polymorphic-type-all --generate-serialization --generate-ostream --generate-doxygen --generate-default-ctor --custom-type double=double --custom-type "decimal=long double" --hxx-epilogue-file ./xml-schema-epilogue.hxx ../../xsd/BioGearsDataModel.xsd

../../../../lib/xsd-4.0.0-i686-macosx/bin/xsd cxx-tree --std c++11 --generate-xml-schema --generate-polymorphic --polymorphic-type-all --generate-serialization --generate-ostream --generate-doxygen --generate-default-ctor data-model-schema.xsd

elif [[ $1 = "linux" ]]
then
../../../../lib/xsd-4.0.0-x86_64-linux-gnu/bin/xsd cxx-tree --std c++11 --file-per-type --extern-xml-schema data-model-schema.xsd --cxx-prologue "#include \"biogears-cdm.hxx\"" --generate-polymorphic --polymorphic-type-all --generate-serialization --generate-ostream --generate-doxygen --generate-default-ctor --custom-type double=double --custom-type "decimal=long double" --hxx-epilogue-file ./xml-schema-epilogue.hxx --export-symbol "__attribute__ ((visibility (\"default\")))" ../../xsd/BioGearsDataModel.xsd

../../../../lib/xsd-4.0.0-x86_64-linux-gnu/bin/xsd cxx-tree --std c++11 --generate-xml-schema --generate-polymorphic --polymorphic-type-all --generate-serialization --generate-ostream --generate-doxygen --generate-default-ctor --export-symbol "__attribute__ ((visibility (\"default\")))" data-model-schema.xsd

else
    echo "Error, expected usage: buildXSD.sh [osx|linux]"
    exit 1
fi

cd ../../..
