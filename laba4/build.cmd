@echo off

cd device_simulation_tool

mkdir build
cd build

cmake -G "MinGW Makefiles" ..

cmake --build .

device_simulation_tool.exe