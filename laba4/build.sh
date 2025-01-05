#!/bin/bash

mkdir build
cd build

cmake ../temperature_logger
cmake --build .

./temperature_logger