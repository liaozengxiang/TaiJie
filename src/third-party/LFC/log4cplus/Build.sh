#!/bin/sh

# 如果 liblog4cplus.a 没有生成，则先编译开源库
if [ ! -f `pwd`/Lib/liblog4cplus.a ]; then
	tar -zxvf log4cplus-1.2.0.tar.gz
	cd log4cplus-1.2.0/
	./configure --prefix=`pwd`/../__INSTALL --enable-static
	make && make install

	# 查看目标文件是否已生成
	cd ..
	if [ ! -f __INSTALL/lib/liblog4cplus.a ]; then
		exit 1;
	fi

    # 将头文件和目标文件拷贝到 Lib、Include 目录下
	mkdir Include Lib
    cp -f __INSTALL/lib/liblog4cplus.a Lib/
    cp -r __INSTALL/include/log4cplus/* Include/
fi
