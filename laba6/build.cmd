@echo off

mkdir build
cd build

qmake laba6.pro
nmake
laba6.exe