#!/bin/bash

# 将 Common/log4cplus 库的头文件软链接到 LFC 目录下
mkdir LFC
cd LFC
ln -s ../../Common/Include Common
ln -s ../../log4cplus/Include log4cplus
cd ..

if [ ! -d Build/ ]; then
	mkdir Build
fi

cd Build
cmake ..
make

# 删除动态生成的 LFC 目录
cd ..
rm -rf LFC
