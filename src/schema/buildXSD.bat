cls
cd .\cpp
mkdir bind
cd bind
del /Q /F *.hxx
del /Q /F *.cxx
xcopy ..\custom-double\*.hxx *

@echo off
set tPATH=%PATH%
set PATH=%XSD%\bin;%PATH%

:: http://www.codesynthesis.com/projects/xsd/documentation/xsd.xhtml

@echo on

:FilePerType
xcopy ..\custom-double\biogears-cdm.cxx *
:: cxx file per type, should use precompiled headers with this
set COMPILE= call "../../../../lib/xsd-4.0.0-i686-windows/bin/xsd.exe" cxx-tree --std c++11 --file-per-type --extern-xml-schema data-model-schema.xsd --cxx-prologue #include\"biogears-cdm.hxx\" --generate-polymorphic --polymorphic-type-all --generate-serialization --generate-ostream --generate-doxygen --generate-default-ctor --custom-type double=double --custom-type "decimal=long double" --hxx-epilogue-file ./xml-schema-epilogue.hxx --export-symbol __declspec(dllexport)
%COMPILE% ..\..\xsd\BioGearsDataModel.xsd
set COMPILE= call "..\..\..\..\lib\xsd-4.0.0-i686-windows\bin\xsd.exe" cxx-tree --std c++11 --generate-xml-schema --generate-polymorphic --polymorphic-type-all --generate-serialization --generate-ostream --generate-doxygen --generate-default-ctor --export-symbol __declspec(dllexport)
%COMPILE% data-model-schema.xsd

:FilePerXSDFile
mkdir min
cd min
del /Q /F *.hxx
del /Q /F *.cxx
xcopy ..\..\custom-double\*.hxx *
:: generate a cxx for each xsd file, faster compiling but makes for very large files that gcc chokes on in linking, works for msvc though...
set COMPILE= call "..\..\..\..\..\..\lib\xsd-4.0.0-i686-windows\bin\xsd.exe" cxx-tree --std c++11 --generate-polymorphic --polymorphic-type-all --generate-serialization --generate-ostream --generate-doxygen --generate-default-ctor --custom-type double=double --custom-type "decimal=long double" --hxx-epilogue-file ..\xml-schema-epilogue.hxx --export-symbol __declspec(dllexport)
mkdir cdm
cd cdm
%COMPILE% ..\..\..\..\xsd\cdm\*.xsd
cd .. 
mkdir biogears
cd biogears
%COMPILE% ..\..\..\..\xsd\biogears\*.xsd

:end
@echo off
set PATH=%tPATH%
cd ../../../
