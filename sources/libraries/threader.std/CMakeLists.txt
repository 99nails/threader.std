cmake_minimum_required(VERSION 3.16)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/threads
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
)

# threader-std

set(THREADER_SOURCES
    include/threads/HandlerBase.h sources/threads/HandlerBase.cpp
    include/threads/HandlerSerialPort.h sources/threads/HandlerSerialPort.cpp
    include/threads/HandlerTcpSocket.h sources/threads/HandlerTcpSocket.cpp
    include/threads/ListThreads.h sources/threads/ListThreads.cpp
    include/threads/MessageBase.h sources/threads/MessageBase.cpp
    include/threads/MessageBinary.h sources/threads/MessageBinary.cpp
    include/threads/MessageLog.h sources/threads/MessageLog.cpp
    include/threads/MessageTimer.h sources/threads/MessageTimer.cpp
    include/threads/MessageConsumer.h sources/threads/MessageConsumer.cpp
    include/threads/PacketFactoryAsciiLines.h sources/threads/PacketFactoryAsciiLines.cpp
    include/threads/PacketFactoryBase.h sources/threads/PacketFactoryBase.cpp
    include/threads/PollerListenSocket.h sources/threads/PollerListenSocket.cpp
    include/threads/PollerThread.h sources/threads/PollerThread.cpp
    include/threads/PollingPosix.h sources/threads/PollingPosix.cpp
    include/threads/QueueMessages.h sources/threads/QueueMessages.cpp
    include/threads/ThreadBase.h sources/threads/ThreadBase.cpp
    include/threads/ThreadHandler.h sources/threads/ThreadHandler.cpp
    include/threads/ThreadListenSocket.h sources/threads/ThreadListenSocket.cpp
    include/threads/ThreadLog.h sources/threads/ThreadLog.cpp
    include/threads/ThreadTimer.h sources/threads/ThreadTimer.cpp
    include/utils/DateTimeUtils.h sources/utils/DateTimeUtils.cpp
    include/utils/FileUtils.h sources/utils/FileUtils.cpp
    include/utils/Logger.h sources/utils/Logger.cpp
    include/utils/MemoryBlock.h sources/utils/MemoryBlock.cpp
    include/utils/SerialUtils.h sources/utils/SerialUtils.cpp
    include/utils/SignalHandler.h sources/utils/SignalHandler.cpp
    include/utils/SocketUtils.h sources/utils/SocketUtils.cpp
    include/utils/StdStringUtils.h sources/utils/StdStringUtils.cpp
    include/utils/StdThreadWrapper.h sources/utils/StdThreadWrapper.cpp
    include/utils/TrafficCounter.h sources/utils/TrafficCounter.cpp
    include/utils/ThreadsCommon.h
    include/utils/ThreadUtils.h sources/utils/ThreadUtils.cpp
    )

if (WIN32)
   set(THREADER_SOURCES "${THREADER_SOURCES}"
       ";include/threads/PollingWindows.h;sources/threads/PollingWindows.cpp")
endif()

add_library(threader-std SHARED
    ${THREADER_SOURCES}
)

target_compile_features(threader-std PUBLIC cxx_std_17)

set_target_properties(threader-std PROPERTIES VERSION ${VERSION} SOVERSION ${VER_MAJ})

target_link_libraries(threader-std PRIVATE
    -lpthread
#    -lstdc++fs
)

# example1-messages

add_executable(example1-messages
    examples/example1-messages/MainExample1.cpp
)
target_compile_features(example1-messages PUBLIC cxx_std_17)

target_link_libraries(example1-messages
    -lpthread
#    -lstdc++fs
    threader-std
)

#example2-threads

add_executable(example2-threads
    examples/example2-threads/MainExample2.cpp
    examples/example2-threads/ThreadTest.h examples/example2-threads/ThreadTest.cpp
)
target_compile_features(example2-threads PUBLIC cxx_std_17)

target_link_libraries(example2-threads
    -lpthread
#    -lstdc++fs
    threader-std
)

# example3-pingpong

add_executable(example3-pingpong
    examples/example3-pingpong/Example3Common.h
    examples/example3-pingpong/MainExample3.cpp
    examples/example3-pingpong/ThreadPing.h examples/example3-pingpong/ThreadPing.cpp
    examples/example3-pingpong/ThreadPong.h examples/example3-pingpong/ThreadPong.cpp
)
target_compile_features(example3-pingpong PUBLIC cxx_std_17)
# find_package(Qt5 COMPONENTS Core REQUIRED)

target_link_libraries(example3-pingpong
    -lpthread
#    -lstdc++fs
    threader-std
)

# example4-serial

add_executable(example4-serial
   examples/example4-serial/Example4Common.h
   examples/example4-serial/MainExample4.cpp
   examples/example4-serial/ThreadPing.h examples/example4-serial/ThreadPing.cpp
   examples/example4-serial/ThreadPong.h examples/example4-serial/ThreadPong.cpp
)
target_compile_features(example4-serial PUBLIC cxx_std_17)

target_link_libraries(example4-serial
   -lpthread
#   -lstdc++fs
   threader-std
)

# example5-simpleserver

add_executable(example5-simpleserver
   examples/example5-simpleserver/Example5Common.h
   examples/example5-simpleserver/MainExample5.cpp
)
target_compile_features(example5-simpleserver PUBLIC cxx_std_17)

target_link_libraries(example5-simpleserver
   -lpthread
   -lstdc++fs
   threader-std
)
