@echo off
pushd %~dp0\..\
cd ../vendor/assimp/
mkdir build/
cd build
cmake -G "Visual Studio 19" -DLIBRARY_SUFFIX="" ..
popd
PAUSE
