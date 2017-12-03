#!/bin/sh

# Generate Ver.h
PROJECT=TaiJie
VERSION=1.0
BUILD=`svn info | grep "Last Changed Rev:" | awk '{print $4}'`

VER_H=source/Ver.h
echo "#ifndef ___VER__20150611___" >$VER_H
echo "#define ___VER__20150611___" >>$VER_H
echo "" >>$VER_H

echo "#define __PROJECT_NAME__    \""$PROJECT"\"" >>$VER_H
echo "#define __PROJECT_VERSION__ \""$VERSION"\"" >>$VER_H
echo "#define __BUILD_VERSION__   \""$BUILD"\"" >>$VER_H

echo "" >>$VER_H
echo "#endif" >>$VER_H

cat $VER_H

# Compile
if [ ! -d build/ ]; then
	mkdir build
fi

cd build
cmake ..
make
