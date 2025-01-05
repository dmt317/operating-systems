#!/bin/bash

mkdir build_device_simulator
cd build_device_simulator

cmake ../device_simulator
cmake --build .

./device_simulator