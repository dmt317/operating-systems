@echo off

cd device_simulator

mkdir build_device_simulator
cd build_device_simulator

cmake -G "MinGW Makefiles" ..

cmake --build .

device_simulator.exe