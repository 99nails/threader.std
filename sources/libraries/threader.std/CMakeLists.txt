cmake_minimum_required(VERSION 3.16)

project(threader-std LANGUAGES CXX)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/threads
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
)

set(THREADER_SOURCES
    include/threads/Connection.h sources/threads/Connection.cpp
    include/threads/HandlerBase.h sources/threads/HandlerBase.cpp
    include/threads/HandlerSerialPort.h sources/threads/HandlerSerialPort.cpp
    include/threads/HandlerTcpSocket.h sources/threads/HandlerTcpSocket.cpp
    include/threads/HandlerUdpSocket.h sources/threads/HandlerUdpSocket.cpp
    include/threads/ListThreads.h sources/threads/ListThreads.cpp
    include/threads/LogMessagesTemplates.h
    include/threads/MessageBase.h sources/threads/MessageBase.cpp
    include/threads/MessageBinary.h sources/threads/MessageBinary.cpp
    include/threads/MessageConsumer.h sources/threads/MessageConsumer.cpp
    include/threads/MessageLog.h sources/threads/MessageLog.cpp
    include/threads/MessageTimer.h sources/threads/MessageTimer.cpp
    include/threads/MessageWriteToFile.h sources/threads/MessageWriteToFile.cpp
    include/threads/PacketFactoryAsciiLines.h sources/threads/PacketFactoryAsciiLines.cpp
    include/threads/PacketFactoryBase.h sources/threads/PacketFactoryBase.cpp
    include/threads/PollerListenSocketConnection.h sources/threads/PollerListenSocketConnection.cpp
    include/threads/PollerThread.h sources/threads/PollerThread.cpp
    include/threads/PollerUdev.h sources/threads/PollerUdev.cpp
    include/threads/PollingPosix.h sources/threads/PollingPosix.cpp
    include/threads/QueueMessages.h sources/threads/QueueMessages.cpp
    include/threads/ThreadBase.h sources/threads/ThreadBase.cpp
    include/threads/ThreadConnections.h sources/threads/ThreadConnections.cpp
    include/threads/ThreadHandler.h sources/threads/ThreadHandler.cpp
    include/threads/ThreadListenSocketConnection.h sources/threads/ThreadListenSocketConnection.cpp
    include/threads/ThreadLog.h sources/threads/ThreadLog.cpp
    include/threads/ThreadMain.h sources/threads/ThreadMain.cpp
    include/threads/ThreadTimer.h sources/threads/ThreadTimer.cpp
    include/threads/ThreadUdevMonitor.h sources/threads/ThreadUdevMonitor.cpp
    include/utils/CommandLineParser.h
    include/utils/CrcUtils.h sources/utils/CrcUtils.cpp
    include/utils/DaemonApplication.h
    include/utils/DaemonUtils.h sources/utils/DaemonUtils.cpp
    include/utils/DateTimeUtils.h sources/utils/DateTimeUtils.cpp
    include/utils/FileUtils.h sources/utils/FileUtils.cpp
    include/utils/IniFile.h sources/utils/IniFile.cpp
    include/utils/Logger.h sources/utils/Logger.cpp
    include/utils/MemoryBlock.h sources/utils/MemoryBlock.cpp
    include/utils/SerialUtils.h sources/utils/SerialUtils.cpp
    include/utils/SettingsBase.h sources/utils/SettingsBase.cpp
    include/utils/SignalHandler.h sources/utils/SignalHandler.cpp
    include/utils/SocketUtils.h sources/utils/SocketUtils.cpp
    include/utils/StdStringUtils.h sources/utils/StdStringUtils.cpp
    include/utils/StdThreadWrapper.h sources/utils/StdThreadWrapper.cpp
    include/utils/TrafficCounter.h sources/utils/TrafficCounter.cpp
    include/utils/ThreadsCommon.h
    include/utils/ThreadUtils.h sources/utils/ThreadUtils.cpp
    include/utils/UdevEnumerator.h sources/utils/UdevEnumerator.cpp
    )

if (WIN32)
   set(THREADER_SOURCES "${THREADER_SOURCES}"
       ";include/threads/PollingWindows.h;sources/threads/PollingWindows.cpp")
endif()

add_library(${PROJECT_NAME} SHARED ${THREADER_SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${VERSION} SOVERSION ${VER_MAJ})

set(LINK_LIBRARIES -lpthread -ludev)

target_link_libraries(${PROJECT_NAME}
    ${LINK_LIBRARIES}
)
