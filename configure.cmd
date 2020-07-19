@echo off
pushd %~dp0vendor\assimp\
mkdir build\
cd build
cmake -G "Visual Studio 16 2019" -DLIBRARY_SUFFIX="" ..
popd
PAUSE
