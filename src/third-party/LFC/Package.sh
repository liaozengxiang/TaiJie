#!/bin/bash

#
# 打包脚本，将 LFC 打包
#

LFC_Version="V100R001B021"
LFC_Folder=`pwd`

# 创建 Lib 目录和 Include 目录
rm -rf LFC
rm -f LFC*.tar.gz
mkdir -p LFC/Lib LFC/Include

# 编译静态库子函数
function CompileLibrary()
{
	LibName=$1

	cd ${LFC_Folder}/${LibName}
	rm -f Lib/lib${LibName}.a
	make
	if [ ! -f Lib/lib${LibName}.a ]; then
		echo "Failed to compile lib${LibName}.a";
		return 1;
	fi

	cp    Lib/lib${LibName}.a ${LFC_Folder}/LFC/Lib/
	chmod 644 Include/*.h
	mkdir ${LFC_Folder}/LFC/Include/${LibName}/
	cp    Include/*.h ${LFC_Folder}/LFC/Include/${LibName}/
	return 0;
}

# 编译所有库，库与库之间使用空格隔开，按先后顺序进行编译
Libs="Common Log Lua Framework Http MySQL"
for LibName in $Libs
do
	CompileLibrary $LibName;
	if [ $? -ne 0 ]; then
		exit 1;
	fi
done

# 打包
cd ${LFC_Folder}
OS_BIT=`uname -r`
LANG=en_US.UTF-8
SVN_VER=`svn info | grep "Last Changed Rev" | awk '{print $4}'`
PACKET_NAME=LFC_${LFC_Version}-svn.${SVN_VER}-${OS_BIT}.tar.gz

tar -zcvf ${PACKET_NAME} LFC/
rm -rf LFC/
