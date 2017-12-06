#!/bin/bash

# 如果 /usr/include/mysql/mysql.h 没有找到，则直接报错
if [ ! -f /usr/include/mysql/mysql.h ]; then
    echo 'Please install mysql header files first';
    exit 1;
fi

# 将 Common 库的头文件软链接到 LFC 目录下
mkdir LFC
cd LFC
ln -s ../../Common/Include Common
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
