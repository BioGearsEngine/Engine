@ECHO OFF

if(%1)==(build) GOTO Start
if(%1)==(clean) GOTO Start
ECHO usage: [build^|clean] [debug^|release]
GOTO Error
if(%2)==(debug) GOTO Start
if(%2)==(release) GOTO Start
ECHO usage: [build^|clean] [debug^|release]
GOTO Error

:Start

SET START_PATH=%PATH%
:: Are we using MSVS 2015?
IF DEFINED %VS140COMNTOOLS% (
  ECHO Using MS Visual Studio 2015
  call "%VS140COMNTOOLS%vsvars32.bat" 
  SET MSPROJ=vcxproj

  IF (%1)==(build) ( 
      SET CPP_BUILD_CMD=msbuild /nologo /p:Configuration=%2 /t:%1
  ) ELSE IF (%1)==(clean) ( 
      SET CPP_BUILD_CMD=msbuild /nologo /p:Configuration=%2 /t:%1
  )
  
  SET TGT_FOLDER=msvs14x32
  IF(%4)==(x64) (
  	SET TGT_FOLDER=msvs14x64
  )
  GOTO Compile
) 
:: See if we using MSVS 2013...
IF DEFINED %VS120COMNTOOLS% (
  ECHO Using MS Visual Studio 2013
  call "%VS120COMNTOOLS%vsvars32.bat" 
  SET MSPROJ=vcxproj

  IF (%1)==(build) ( 
      SET CPP_BUILD_CMD=msbuild /nologo /p:Configuration=%2 /t:%1
  ) ELSE IF (%1)==(clean) ( 
      SET CPP_BUILD_CMD=msbuild /nologo /p:Configuration=%2 /t:%1
  )
  
  SET TGT_FOLDER=msvs12x32
  IF(%4)==(x64) (
  	SET TGT_FOLDER=msvs12x64
  )
  GOTO Compile
) 

ECHO Build Requires MSVC 2013 or MSVC 2015 installed, neither found
GOTO Error

:Compile

:: Do Not have the .sln in the same directory as the .v*proj
:: Just keep the .sln in the same code root directory as the java project
:: This will speed up the build as devenv will compile the whole .sln if it finds it :(

GOTO here
:here


:: Build Projects %1 = build or clean; %2 = debug or release
%CPP_BUILD_CMD% "./cmake/%TGT_FOLDER%/BioGears.sln
if ERRORLEVEL 1 GOTO Error

:End

ECHO Build Successful
set PATH=%START_PATH%
EXIT /B 0

:Error
set PATH=%START_PATH%
:: If we were called from ant, kill the cmd process
if(%3)==(ant) EXIT  1
EXIT /B 1
