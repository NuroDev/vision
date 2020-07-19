@echo off
pushd %~dp0vendor\assimp\
mkdir build\
cd build
cmake -G "Visual Studio 16 2019" -DLIBRARY_SUFFIX="" -DCMAKE_DEBUG_POSTFIX="" .. .. 
popd
PAUSE
