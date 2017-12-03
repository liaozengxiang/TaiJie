CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_CXX_FLAGS         "-g3 -Wall")
SET(CMAKE_CXX_FLAGS_DEBUG   "-g3 -Wall")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -DNDEBUG")
SET(LIBRARY_OUTPUT_PATH     ${PROJECT_SOURCE_DIR}/Lib)
INCLUDE_DIRECTORIES(LFC)

SET(SOURCE_FILES
	Source/RtspClientEvent.cpp
	Source/RtspCode.cpp
	Source/RtspFrameServer.cpp
	Source/RtspMessage.cpp
	Source/RtspMessageParser.cpp
	Source/RtspReqMessage.cpp
	Source/RtspResMessage.cpp
)

SET(INCLUDE_FILES
	Include/RtspClientEvent.h
	Include/RtspCode.h
	Include/RtspFrameServer.h
	Include/RtspMessage.h
	Include/RtspMessageParser.h
	Include/RtspReqMessage.h
	Include/RtspResMessage.h
)

ADD_LIBRARY(Rtsp STATIC ${SOURCE_FILES} ${INCLUDE_FILES})
