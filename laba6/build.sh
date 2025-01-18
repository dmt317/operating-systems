#!/bin/bash

mkdir build
cd build

qmake ../

make -j$(nproc)

cd laba6.app/Contents/MacOS

./laba6