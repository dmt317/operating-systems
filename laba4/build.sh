#!/bin/bash

mkdir build
cd build

cmake ../device_simulation_tool

cmake --build .

./device_simulation_tool