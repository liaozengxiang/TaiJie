CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_CXX_FLAGS         "-g3 -Wall")
SET(CMAKE_CXX_FLAGS_DEBUG   "-g3 -Wall")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -DNDEBUG")
SET(LIBRARY_OUTPUT_PATH     ${PROJECT_SOURCE_DIR}/Lib)

SET(SOURCE_FILES
	Source/Archive.cpp
	Source/Argv.cpp
	Source/Assert.cpp
	Source/Base64.cpp
	Source/Buffer.cpp
	Source/Event.cpp
	Source/EventEngine.cpp
	Source/EventThread.cpp
	Source/FileWatch.cpp
	Source/InetListenSocket.cpp
	Source/InetSocket.cpp
	Source/Listener.cpp
	Source/MemLeakDetect.cpp
	Source/MessageEvent.cpp
	Source/Mutex.cpp
	Source/OS.cpp
	Source/RWLock.cpp
	Source/Semaphore.cpp
	Source/SocketAddr.cpp
	Source/Socket.cpp
	Source/SocketUtil.cpp
	Source/STR.cpp
	Source/TCPClientSocket.cpp
	Source/TCPSocket.cpp
	Source/Thread.cpp
	Source/ThreadPool.cpp
	Source/Timer.cpp
	Source/Tinyxml2.cpp
	Source/UDPSocket.cpp
	Source/UnixListenSocket.cpp
	Source/UnixSocket.cpp
	Source/UrlCodec.cpp
	Source/XmlParser.cpp
)

SET(INCLUDE_FILES
	Include/Archive.h
	Include/Argv.h
	Include/Assert.h
	Include/Base64.h
	Include/Buffer.h
	Include/Common.h
	Include/EventEngine.h
	Include/EventThread.h
	Include/Event.h
	Include/FileWatch.h
	Include/InetListenSocket.h
	Include/InetSocket.h
	Include/Listener.h
	Include/Macro.h
	Include/MemLeakDetect.h
	Include/MemoryPool.h
	Include/MessageEvent.h
	Include/Mutex.h
	Include/OS.h
	Include/OSType.h
	Include/Reference.h
	Include/RWLock.h
	Include/Semaphore.h
	Include/Singleton.h
	Include/SocketAddr.h
	Include/Socket.h
	Include/SocketUtil.h
	Include/STR.h
	Include/TCPClientSocket.h
	Include/TCPSocket.h
	Include/Thread.h
	Include/ThreadPool.h
	Include/Timer.h
	Include/Tinyxml2.h
	Include/UDPSocket.h
	Include/UnixListenSocket.h
	Include/UnixSocket.h
	Include/UrlCodec.h
	Include/XmlParser.h
)

ADD_LIBRARY(Common STATIC ${SOURCE_FILES} ${INCLUDE_FILES})
