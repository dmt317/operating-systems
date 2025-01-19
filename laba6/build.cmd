@echo off

mkdir build
cd build

qmake laba6.pro
make

cd release

laba6.exe