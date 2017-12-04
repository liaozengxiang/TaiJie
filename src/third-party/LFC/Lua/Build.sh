#!/bin/bash

# 如果 liblua.a 没有编译，则先编译开源库
if [ ! -f /usr/local/lib/liblua.a ]; then
	tar -zxvf lua-5.3.0.tar.gz
	cd lua-5.3.0
	make linux
	make install
	cd ..
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
