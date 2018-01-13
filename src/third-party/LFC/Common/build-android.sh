#!/bin/sh

mkdir -p output/include
rm -f CMakeLists.txt
cp CMakeLists-android.txt CMakeLists.txt
if [ ! -d build ]; then
    mkdir build
fi
cd build
cmake ..
make

cd ..
rm -rf output/include/*
cp -ra Include output/include/Common
