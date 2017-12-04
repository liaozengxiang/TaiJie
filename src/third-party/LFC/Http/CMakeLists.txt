CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_CXX_FLAGS         "-g3 -Wall")
SET(CMAKE_CXX_FLAGS_DEBUG   "-g3 -Wall")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -DNDEBUG")
SET(LIBRARY_OUTPUT_PATH     ${PROJECT_SOURCE_DIR}/Lib)
INCLUDE_DIRECTORIES(LFC)

SET(SOURCE_FILES
	Source/HttpClient.cpp
	Source/HttpClientEvent.cpp
	Source/HttpCode.cpp
	Source/HttpFrameServer.cpp
	Source/HttpMessage.cpp
	Source/HttpParser.c
	Source/HttpReqMessage.cpp
	Source/HttpResMessage.cpp
)

SET(INCLUDE_FILES
	Include/HttpClient.h
	Include/HttpClientEvent.h
	Include/HttpCode.h
	Include/HttpFrameServer.h
	Include/HttpMessage.h
	Include/HttpParser.h
	Include/HttpReqMessage.h
	Include/HttpResMessage.h
)

ADD_LIBRARY(Http STATIC ${SOURCE_FILES} ${INCLUDE_FILES})
