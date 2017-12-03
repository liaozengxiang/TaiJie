CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_CXX_FLAGS         "-g3 -Wall")
SET(CMAKE_CXX_FLAGS_DEBUG   "-g3 -Wall")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -DNDEBUG")
SET(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../third-party/LFC/LFC/Include)

aux_source_directory(source/session/ FILES)
aux_source_directory(source/startup/ FILES)
aux_source_directory(source/user/    FILES)

ADD_EXECUTABLE(TaiJie ${FILES})
TARGET_LINK_LIBRARIES(TaiJie
                      ${PROJECT_SOURCE_DIR}/../third-party/LFC/LFC/Lib/libCommon.a
                      ${PROJECT_SOURCE_DIR}/../third-party/LFC/LFC/Lib/libLog.a
                      /usr/local/lib/liblog4cplus.a
                      -lpthread -lrt -ldl)
