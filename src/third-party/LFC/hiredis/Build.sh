#!/bin/sh

tar zxvf redis-4.0.1.tar.gz
cd redis-4.0.1
make

# 查看目标文件是否已生成
if [ ! -f deps/hiredis/libhiredis.a ]; then
    echo 'Failed to compile libhiredis.a';
    exit 1;
fi

# 将静态库文件拷贝到目标目录
mkdir ../Include ../Lib
cp -rf deps/hiredis/libhiredis.a ../Lib/
cp -rf deps/hiredis/*.h ../Include/
