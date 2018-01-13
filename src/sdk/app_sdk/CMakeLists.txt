CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_SYSTEM_NAME  Android)
SET(CMAKE_CXX_COMPILER "arm-linux-androideabi-g++")
SET(CMAKE_C_COMPILER   "arm-linux-androideabi-gcc")

SET(CMAKE_CXX_FLAGS    "-g3 -Wall -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 --sysroot=/usr/local/android-ndk-r10e/platforms/android-21/arch-arm")
SET(CMAKE_C_FLAGS      "-g3 -Wall -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 --sysroot=/usr/local/android-ndk-r10e/platforms/android-21/arch-arm")
SET(CMAKE_LD_FLAGS     "-L/usr/local/android-ndk-r10e/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a -L${PROJECT_SOURCE_DIR}/../../third-party/LFC/Common/output/lib/android -lcommon -lgnustl_shared")

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../third-party/LFC/Common/output/include)
INCLUDE_DIRECTORIES(/usr/local/android-ndk-r10e/sources/cxx-stl/gnu-libstdc++/4.9/include)
INCLUDE_DIRECTORIES(/usr/local/android-ndk-r10e/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/include)

aux_source_directory(source/game/       FILES)
aux_source_directory(source/parser/     FILES)
aux_source_directory(source/socket/     FILES)


ADD_LIBRARY(app_sdk SHARED ${FILES})
TARGET_LINK_LIBRARIES(app_sdk -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 ${CMAKE_LD_FLAGS})
