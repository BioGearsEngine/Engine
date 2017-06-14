@echo off
rem file      : examples/tester.bat
rem copyright : Copyright (c) 2013-2014 Code Synthesis Tools CC
rem license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

rem
rem Run example. The example directory is the current directory.
rem
rem %1     configuration, for example, Debug or Release
rem %2     platform, for example Win32 or x64
rem

setlocal

if "_%DIFF%_" == "__" set DIFF=fc

if "_%2_" == "_Win32_" (
  set "dir=%1"
) else (
  set "dir=%2\%1"
)

rem Globbing returns files in alphabetic order. We also need to cover
rem compressed XML files (.xml.gz).
rem
if exist *.xml* (
  for %%f in (*.xml*) do (
    if NOT "%%f" == "out.xml" (
      if NOT "%%f" == "out.xml.gz" (
        %dir%\driver.exe %%f
        if errorlevel 1 goto error
      )
    )
  )
) else (
  %dir%\driver.exe
  if errorlevel 1 goto error
)

goto end

:error
endlocal
exit /b 1

:end
endlocal
