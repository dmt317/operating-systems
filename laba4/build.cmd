@echo off

cd temperature_logger

mkdir build
cd build

cmake -G "MinGW Makefiles" ..

cmake --build .

temperature_logger.exe