#!/bin/sh

# 如果 libmysqlclient.a/libmysqlclient.so 没有生成，则直接报错
if [ ! -f /usr/lib64/mysql/libmysqlclient.a ] &&
   [ ! -f /usr/lib64/mysql/libmysqlclient.so ] &&
   [ ! -f /usr/lib/mysql/libmysqlclient.a ] &&
   [ ! -f /usr/lib/mysql/libmysqlclient.so ]; then
    echo 'Please install libmysqlclient.a/libmysqlclient.so first';
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
