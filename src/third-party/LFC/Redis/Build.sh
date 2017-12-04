#!/bin/bash

mkdir LFC
cd LFC
ln -s ../../Common/Include Common
ln -s ../../hiredis/Include hiredis
cd ..

if [ ! -d Build/ ]; then
	mkdir Build
fi

cd Build
cmake ..
make

cd ..
rm -rf LFC
