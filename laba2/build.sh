#!/bin/bash

mkdir build && cd build

cmake ../bg_process_manager

cmake --build .

./test_runner