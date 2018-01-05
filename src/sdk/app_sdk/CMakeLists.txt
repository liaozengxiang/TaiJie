CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_CXX_FLAGS         "-g3 -Wall")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../third-party/LFC/LFC/Include)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../third-party/LFC/LFC/Lib)

aux_source_directory(source/game/       FILES)
aux_source_directory(source/parser/     FILES)
aux_source_directory(source/socket/     FILES)

LINK_LIBRARIES(Common rt)
ADD_EXECUTABLE(app_sdk ${FILES})
