#!/bin/sh

# 如果 /usr/local/lib/liblog4cplus.a 没有生成，则先编译开源库
if [ ! -f /usr/local/lib/liblog4cplus.a ]; then
	tar -zxvf log4cplus-1.2.0.tar.gz
	cd log4cplus-1.2.0/
	./configure --prefix=/usr/local/ --enable-static
	make

	# 查看目标文件是否已生成
	if [ ! -f .libs/liblog4cplus.a ]; then
		exit 1;
	fi

	# 将静态库、动态库、头文件生成到目标目录下
	make install

	# 退出开源库目录，准备编译 libLog.a
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
