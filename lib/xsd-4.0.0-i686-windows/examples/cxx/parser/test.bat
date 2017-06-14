@echo off
rem file      : examples/test.bat
rem copyright : Copyright (c) 2013-2014 Code Synthesis Tools CC
rem license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

setlocal

set "tests=generated hello library mixin mixed multiroot performance polymorphism polyroot wildcard"
set "confs=Debug Release"
set "plats=Win32 x64"
set "curdir=%CD%"
set "topdir=%curdir%\..\.."
set "failed="

goto start

rem
rem %1 - test directory
rem %2 - configuration
rem %3 - platform
rem
:run_test
  cd %1

  if "_%3_" == "_Win32_" (
    set "dir=%2"
  ) else (
    set "dir=%3\%2"
  )

  if exist %dir%\driver.exe (
    echo %1\%3\%2
    call %topdir%\tester.bat %2 %3
    if errorlevel 1 (
      set "failed=%failed% %1\%3\%2"
    )
  )

  cd %curdir%
goto :eof

:start

for %%t in (%tests%) do (
  for %%c in (%confs%) do (
    for %%p in (%plats%) do (
      call :run_test %%t %%c %%p
    )
  )
)

if not "_%failed%_" == "__" goto error

echo.
echo ALL EXAMPLES PASSED
echo.
goto end

:usage
echo.
echo usage: test.bat
echo.

:error
if not "_%failed%_" == "__" (
  echo.
  for %%t in (%failed%) do echo FAILED: %%t
  echo.
)
endlocal
exit /b 1

:end
endlocal
