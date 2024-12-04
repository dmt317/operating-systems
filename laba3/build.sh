#!/bin/bash

mkdir build
cd build

cmake ../cross_platform_logger

cmake --build .

./cross_platform_logger