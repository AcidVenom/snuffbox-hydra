@echo off

set OLD_PATH=%PATH%
set PATH=%~dp0depot_tools;%PATH%

set DEPOT_TOOLS_WIN_TOOLCHAIN=0
set GYP_MSVS_VERSION=2017

if exist .\v8 (
  echo Removing old '.\v8' folder as we're regenerating
  rmdir .\v8 /s
  if exist .gclient del /q /f .gclient
  if exist .gclient_entries del /q /f .gclient_entries
)

echo Fetching v8 with gclient..

if not exist .\v8 mkdir .\v8

call gclient & echo.

call gclient config https://chromium.googlesource.com/v8/v8 & echo.
call fetch v8 & echo.

call gclient sync & echo.

echo Configuring v8 for compilation

cd .\v8

call python "tools/dev/v8gen.py" x64.debug -vv
call python "tools/dev/v8gen.py" x64.release -vv

copy /Y ..\gn\x64.debug.win32.gn .\out.gn\x64.debug\args.gn
copy /Y ..\gn\x64.release.win32.gn .\out.gn\x64.release\args.gn & echo.

set /P compile="Compile V8? Please note this can take quite a while (y/n)"
if /I "%compile%" neq "y" goto skipcompile

call ninja -C "out.gn/x64.debug"
call ninja -C "out.gn/x64.release"

cd ..\

:skipcompile

echo Succesfully fetched/built v8
set PATH=%OLD_PATH%