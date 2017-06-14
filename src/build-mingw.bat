@ECHO OFF

SET START_PATH=%PATH%

if(%1)==(build) GOTO Start
if(%1)==(clean) GOTO Start
ECHO usage: [build^|clean] [debug^|release]
GOTO Error
if(%2)==(debug) GOTO Start
if(%2)==(release) GOTO Start
ECHO usage: [build^|clean] [debug^|release]
GOTO Error

:Start

IF EXIST ./cmake/mingw/%2 (
    cd cmake/mingw/%2
) ELSE (
    IF "%1"=="clean" GOTO EndClean
    
    ECHO CMake directory does not exist, run ant cmake -Denv=mingw in BioGears/src to generate the make files
    GOTO Error
)

IF (%1)==(build) ( 
    start /B /WAIT mingw32-make
) ELSE IF (%1)==(clean) ( 
    start /B /WAIT "" "mingw32-make" clean
)
IF ERRORLEVEL 1 (
ECHO Unable to execute mingw32-make.exe
GOTO Error
) ELSE IF (%1)==(build) (
    GOTO EndBuild
) ELSE IF (%1)==(clean) (
    GOTO EndClean
)

:EndBuild
ECHO Build Successful
set PATH=%START_PATH%
EXIT /B 0

:EndClean
ECHO Clean Successful
set PATH=%START_PATH%
EXIT /B 0

:Error
set PATH=%START_PATH%
:: If we were called from ant, kill the cmd process
if(%3)==(ant) EXIT  1
EXIT /B 1
