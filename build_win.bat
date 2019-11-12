@echo off

set path_to_source=%CD%\\%1
echo %path_to_source%

pushd %path_to_source%
mkdir build & pushd build
mkdir win & pushd win

if exist x64 (
    rmdir /s /q x64
)

if exist win32 (
    rmdir /s /q win32
)

mkdir x64 & pushd x64
cmake -G "Visual Studio 16 2019" -A x64 %path_to_source%
cmake --build . --config Debug
REM cmake --build . --config Release

popd @rem x64
popd @rem win
popd @rem build
popd @rem path_to_source