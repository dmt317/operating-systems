@echo off

cd bg_process_manager

mkdir build
cd build

cmake -G "MinGW Makefiles" ..

cmake --build .

test_runner.exe