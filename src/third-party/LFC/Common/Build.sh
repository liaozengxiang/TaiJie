#!/bin/bash

if [ ! -d Build/ ]; then
	mkdir Build
fi

cp CMakeLists-linux.txt CMakeLists.txt
cd Build
cmake ..
make
