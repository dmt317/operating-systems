@echo off

cd device_simulation_tool

mkdir build_device_simulator
cd build_device_simulator

cmake -G "MinGW Makefiles" ..

cmake --build .

device_simulator.exe