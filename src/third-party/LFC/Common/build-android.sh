#!/bin/sh

rm -f CMakeLists.txt
cp CMakeLists-android.txt CMakeLists.txt
if [ ! -d build ]; then
    mkdir build
fi
cd build
cmake ..
make
