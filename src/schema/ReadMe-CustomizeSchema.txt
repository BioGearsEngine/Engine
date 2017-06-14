Creating a custom engine schema

If you are creating a whole new physiology engine and would like to utilize the common data model,
replace the BioGears data model with a data model for your engine, this is what you will need to do:
(use BioGears as an example)

1. Create your own root level schema and a folder for your schema in the ./xsd folder
  a. I will use NewDataModel.xsd as the name of your new data model
2. Update the buildXSD.xml ant file with your new root schema file name (NewDataModel.xsd)
  a. This ant file is used to call Java's jaxb to generate bind classes
     Even if you are not planning on supporting Java, I find generating Java bind classes
	 much more forgiving that C++, you don't have to fight recursive headers includes while
	 you ensure all your schema objects are of proper form, it's an easier bar to reach 
	 in creating a valid schema for your phyisiology engine
3. Update the C++/Code Synthesis infrastructure to build C++ binding classes
  a. Make your own root include files
    1. the ./cpp/bind holds a folder 'custom-double' this is used to ensure we write out 
       full precision rather than scientific notation in the xml files written by our bind classes
    2. Create your own versions of the biogears-cdm.cxx and biogears.hxx files
	   These files are used in the CodeSynthesis generation script to be include first, by all generated classes
  b. Update the CodeSynthesis scripts (buildXSD.bat for windows, and buildXSD.sh for linux and osx)
    1. Update the copy call in the script to copy your custom cxx file from 3.a.2 
	  a. Windows : xcopy .\custom-double\biogears-cdm.cxx *
	  b. Unix : cp ./custom-double/biogears-cdm.cxx .	
    2. Replace the --cxx-prologue #include\"biogears-cdm.hxx\" with the include you made in 3.a.2
	3. Update the root schema file with your new root schema file
	  a. Change instances of BioGearsDataModel.xsd to NewDataModel.xsd
	4. For windows, update this folder to point to your new folder
	  a. %COMPILE% ..\..\..\xsd\biogears\*.xsd
  
