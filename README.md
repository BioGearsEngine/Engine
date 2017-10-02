Overview
==========

BioGears® source code is hosted here and will be developed through this public repository to encourage community facing development. In addition we also support official stable releases. Our latest deployment (found under releases) is still in a beta phase, and is intended to be an intermediate release to showcase the capabilities of the BioGears® Physiology Engine. The current version of the software includes examples of all types of engine interfaces, but does not include all of the functionality or physiologic systems that will be present in the final product. This version of the software is meant to elicit feedback and enhance community involvement in establishing end product expectations.

What is BioGears®
------------------
BioGears®  is a C++ based, open source, multi-platform (Windows, Mac, and Linux), comprehensive human physiology engine that will drive medical education, research, and training technologies. BioGears enables accurate and consistent physiology simulation across the medical community. The engine can be used as a standalone application or integrated with simulators, sensor interfaces, and models of all fidelities.

BioGears high-level objectives
--------------------------------
* Create a publicly available physiology research platform that enables accurate and consistent simulated physiology across training applications
* Lower the barrier to create medical training content
* Engage the community to develop and extend physiology models
* Meet the training needs of the military
* Expand the body of knowledge regarding the use of simulated physiology for medical education

Building Source
======

Dependancies 
-------------
Biogears® engine relies on the following dependancies: 

* Cmake 3.0 or greater
* Apache Ant
* JDK 1.7 or greater

Please make sure that all dependancies are installed and linked properly to your PATH before building. 

Build Instruction
-------------------

### Windows 
Open a command window, navigate to the \src folder in the BioGears project and type 

~~~~~
ant cmake
~~~~~

This will build the visual studio solution file in the \src\cmake, titled Biogears.sln. Open this file in visual studio then build to compile the code. All builds copy their compiled targets to /bin/release or /bin/debug depending on MSVC build configuration. Execution and debugging working directories should be set to the bin directory. If you don't wish to use visual studio as your build tool, be sure to have the microsoft compilers on your computer and in the \src folder type 

`ant compile`


This will create the neccessary .dll files and a scenario executor file to run BioGears through the command line.

When submitting a rather large pull request (these generally include major model changes) we ask that you update the appropriate methodology file so that we can properly review and document your changes. Go into the /Methodology folder and open the appropriate file to include your model updates.
### Building with Xcode on Mac OS X  XCODE

#### Xcode

Download and install Xcode from the app store.  After the installation finishes, install the Xcode command line tools by running the
terminal command `xcode-select --install`.

#### Required tools

Make sure the following tools are installed:
- ant
- CMake
- Java JDK 8

An easy way to install ant and CMake is with Homebrew.  First install homebrew by running the command
`ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`.  Next install ant and CMake by running the commands
`brew install ant` and `brew install cmake`.<br/>

#### Build Xerces

Navigate in a terminal window to the xerces-3.1.2 directory in the BioGears lib directory.  Build Xerces by running
`./configure --disable-threads --disable-network --enable-transcoder-macosunicodeconverter --disable-pretty-make CXXFLAGS=-O3 CFLAGS=-O3` and then `make`.
<br/>

After Xerces builds successfully, we have to set the dylib's install path.  Navigate to the xerces-c-3.1.2/src/.libs directory, and run the command
`install_name_tool -id @rpath/libxerces-c-3.1.dylib libxerces-c-3.1.dylib`.  This sets the install path of the dylib to @rpath so that the BioGears executable
will search the current directory when attempting to load it.  Run the command `otool -D libxerces-c-3.1.dylib` to make sure the install path has been
set correctly, this should output `@rpath/libxerces-c-3.1.dylib`.

#### Building and Running BioGears in Xcode

Navigate in a terminal window to the BioGears src directory and run `ant cmake -Denv=xcode`.  This will generate debug and release Xcode project files in
src/cmake/xcode.
<br/>

Open the BioGears.xcodeproj file in Xcode.  Select Product > Scheme > Edit Scheme and select Run on the left column.  Set the following configuration options:

- Set the executable to BioGearsScenarioDriver if it isn't already.
- Set the build configuration to Debug or Release depending on the desired mode.
- On the arguments tab, add an entry to "Arguments Passed On Launch" which specifies a path to a scenario to run (e.g. ../verification/Scenarios/Basic/Basic1.xml).
- On the options tab, check "Use custom working directory" and set it to the BioGears bin folder.

Close the dialog and click the Run button to build and launch the BioGears scenario driver.
<br/>

#### Building and Running BioGears from the Command Line

Navigate in a terminal window to the BioGears src directory and run `ant cmake -Denv=xcode`.  This will generate debug and release Xcode project files in
src/cmake/xcode.
<br/>

To build BioGears from the command line, open the generated xcodeproj file in Xcode.  Opening the project in Xcode will force the project schemes to generate,
these are necessary for command line builds.  After Xcode loads the project it can be closed.  Run the command `ant compile -Denv=xcode` in BioGears's
src directory to build.

After BioGears is built, navigate to the bin directory and launch the scenario driver from there, specifying the scenario you want to run as the first parameter
(e.g. `./Release/BioGearsScenarioDriver ../verification/Scenarios/Basic/Basic1.xml`).


### Building with GCC on Linux  LINUX

#### Required tools

Make sure the following tools are installed:
- ant
- CMake
- Java JDK 8
- GCC

Add a JAVA_HOME variable to point to the Java installation and add it to the system PATH.  This can be done by adding the following lines to ~/.profile:
```
JAVA_HOME=/usr/local/jdk1.8.0_45
PATH=$PATH:$JAVA_HOME_bin
```

Replace the JAVA_HOME value with the correct installation directory.

#### Build Xerces

Navigate in a terminal window to the xerces-3.1.2 directory in the BioGears lib directory.  Build Xerces by running
`./configure --disable-threads --disable-network --enable-transcoder-gnuiconv --disable-pretty-make CXXFLAGS=-O3 CFLAGS=-O3` and then `make`.
<br/>

#### Building BioGears from the Command Line

Navigate in a terminal window to the BioGears src directory and run `ant cmake -Denv=unixMake` and then `ant compile -Denv=unixMake`.

### Cross-compiling with GCC for the Raspberry Pi  RPI

<b>Note:</b> This build has not been thoroughly tested.

This build is very similar to the GCC Linux build, the only difference being that the code is compiled with a version of GCC that produces ARM-compatible objects.

#### Compiler

Install `gcc-arm-linux-gnueabihf` and `g++-arm-linux-gnueabihf` (e.g. using `sudo apt-get install gcc-arm-linux-gnueabihf` etc.).  In addition to this, make sure gcc 4.9 is installed.

#### Build Xerces

Build Xerces by running
`./configure --disable-threads --disable-network --enable-transcoder-gnuiconv --disable-pretty-make CC=/usr/bin/arm-linux-gnueabihf-gcc CXX=/usr/bin/arm-linux-gnueabihf-g++ CXXFLAGS=-O3 CFLAGS=-O3 --host arm-linux` and then `make`.

#### Building BioGears from the Command Line

Navigate in a terminal window to the BioGears src directory and run `ant cmake -Denv=raspberryPi` and then `ant compile -Denv=raspberryPi`.

### Deploying on *nix Platforms

BioGears can be deployed as an executable or as a library to be used in other applications.  Each of the deployment processes below assumes the code has already been built with `ant compile -Denv=your_environment`.

#### Mac Executable

Deploy the Mac executable by running `deploy-unix-executable.sh` in the BioGears src directory.  This will deploy the necessary files to the BioGears deploy/toolkit directory.  To launch the GUI from that directory, run `BioGearsGUI.sh`.

#### Mac SDK

Deploy the Mac SDK by running `deploy-osx-library.sh` in the BioGears src directory.  This will deploy the necessay header files and .dylib files to the BioGears library directory.  The HowTo cpp files provide examples of how to use the BioGears API from your own software.  The build-osx.sh script will build all of the HowTo files and place the resulting executable in the library/bin directory.

#### Linux Executable

Deploy the Linux executable by running `deploy-unix-executable.sh` in the BioGears src directory.  This will deploy the necessary files to the BioGears deploy/toolkit directory.  To launch the GUI from that directory, run `BioGearsGUI.sh`.

#### Linux SDK

Deploy the Linux SDK by running `deploy-linux-library.sh` in the BioGears src directory.  This will deploy the necessay header files and .so files to the BioGears library directory.  The HowTo cpp files provide examples of how to use the BioGears API from your own software.  The build-linux.sh script will build all of the HowTo files and place the resulting executable in the library/bin directory.

Structure 
------------
The BioGears source is structured as follows:
- bin - Contains all data and configuration files needed for execution of the BioGears Engine
- data  - Contains the Microsoft Excel spreadsheets for all BioGears data sets
- lib - Contains third party libraries used by this project (see  Credits for more details)
- src
	- cdm - Code associated with the  CDM and  physeng 
  - cmake - The directory where cmake will create build files
	- engine - Code associated with the lumped parameter models 
    - controller - These classes hold data necessary by the model, control the advancement of time
      - scenario - These classes help execute a BioGears specific scenario (i.e. a scenario with a BioGearsConfiguration object)
        - BioGearsScenario -  BioGearsScenario
        - BioGearsScenarioExec -  BioGearsScenarioExec
        - BioGearsScenarioInitialParameters -  BioGearsScenarioInitialParameters
      - BioGears -  BioGears
      - BioGearsCircuits -  BioGearsCircuits
      - BioGearsCompartments -  BioGearsCompartments
      - BioGearsConfiguration -  BioGearsConfiguration
      - BioGearsEngine -  BioGearsEngine
      - BioGearsSubstances -  BioGearsSubstances
      - BioGearsSystem -  BioGearsSystem
    - systems - These classes implement the methodology for modeling and simulating
      - BloodChemistry -  BloodChemistry
      - Cardiovascular -  Cardiovascular
      - Drugs -  Drugs
      - Endocrine -  Endocrine
      - Energy -  Energy
      - Environment -  Environment
      - Gastrointestinal -  Gastrointestinal
      - Nervous -  Nervous
      - Renal -  Renal
      - Respiratory -  Respiratory		
      - SaturationCalculator -  SaturationCalculator	
      - Tissue -  Tissue
    - equipment
      - AnesthesiaMachine -  AnesthesiaMachineData
      - ECG -  ElectrocardiogramData
      - Inhaler -  InhalerData    
  - schema - The xsd data definitions used by the CDM
	- sdk - Example code and scripts for the  Toolkit and  SDK
	- utils - Various utilities used in validation

Tentative Near-Term Timeline
============================
Planned model updates Oct 2, 2017:

* Exocrine and secretory functions
* Starvation and dehydration states
* Nerve agent model

Programmatics
===============
BioGears is being developed under the TATRC funded project: W81XWH-13-2-0068.

Disclaimer:

This work is supported by the US Army Medical Research and Materiel Command. The views, opinions and/or findings contained in this report are those of the author(s) and should not be construed as an official Department of the Army position, policy, or decision unless so designated by other documentation.
BioGears is released under the Apache 2.0 liscense.

BioGears has Publications papers and abstracts on several systems and clinical scenarios.

Additional Information
======================

Code of Conduct
------------------
We support the [contibutor covenant](https://github.com/BioGearsEngine/Engine/blob/master/CODE_OF_CONDUCT.md) and the scope and enforcement it details. We reserve the right of enforment that we feel is appropriate given the nature of the offense up to and including a permanent ban from the project.


Contributing 
-------------
Details will be filled in shortly. In the meantime if you have a contribution or issue to post/push feel free to contact abaird@ara.com with the details. 

Additional Documentation
--------------------------
For more detailed documentation including model discussions and implementation details can be found at www.BioGearsEngine.com