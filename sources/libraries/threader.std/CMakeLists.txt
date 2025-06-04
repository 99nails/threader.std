cmake_minimum_required(VERSION 3.5)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/threads
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
)

add_library(threader-std SHARED
    include/threads/HandlerBase.h sources/threads/HandlerBase.cpp
    include/threads/ListThreads.h sources/threads/ListThreads.cpp
    include/threads/MessageBase.h sources/threads/MessageBase.cpp
    include/threads/MessageLog.h sources/threads/MessageLog.cpp
    include/threads/MessageTimer.h sources/threads/MessageTimer.cpp
    include/threads/MessageConsumer.h sources/threads/MessageConsumer.cpp
    include/threads/PollerThread.h sources/threads/PollerThread.cpp
    include/threads/PollingPosix.h sources/threads/PollingPosix.cpp
    include/threads/QueueMessages.h sources/threads/QueueMessages.cpp
    include/threads/ThreadBase.h sources/threads/ThreadBase.cpp
    include/threads/ThreadLog.h sources/threads/ThreadLog.cpp
    include/threads/ThreadTimer.h sources/threads/ThreadTimer.cpp
    include/utils/DateTimeUtils.h sources/utils/DateTimeUtils.cpp
    include/utils/FileUtils.h sources/utils/FileUtils.cpp
    include/utils/Logger.h sources/utils/Logger.cpp
    include/utils/MemoryBlock.h sources/utils/MemoryBlock.cpp
    include/utils/StdStringUtils.h sources/utils/StdStringUtils.cpp
    include/utils/StdThreadWrapper.h sources/utils/StdThreadWrapper.cpp
    include/utils/ThreadsCommon.h
    include/utils/ThreadUtils.h sources/utils/ThreadUtils.cpp
)

target_compile_features(threader-std PUBLIC cxx_std_17)
target_compile_definitions(threader-std PRIVATE DDSLIB_LIBRARY)

set_target_properties(threader-std PROPERTIES VERSION ${VERSION} SOVERSION ${VER_MAJ})

add_executable(example1-messages
    examples/example1-messages/MainExample1.cpp
)
target_compile_features(example1-messages PUBLIC cxx_std_17)

add_executable(example2-threads
    examples/example2-threads/MainExample2.cpp
    examples/example2-threads/ThreadTest.h examples/example2-threads/ThreadTest.cpp
)
target_compile_features(example2-threads PUBLIC cxx_std_17)

add_executable(example3-pingpong
    examples/example3-pingpong/Example3Common.h
    examples/example3-pingpong/MainExample3.cpp
    examples/example3-pingpong/ThreadPing.h examples/example3-pingpong/ThreadPing.cpp
    examples/example3-pingpong/ThreadPong.h examples/example3-pingpong/ThreadPong.cpp
)
target_compile_features(example3-pingpong PUBLIC cxx_std_17)
# find_package(Qt5 COMPONENTS Core REQUIRED)

target_link_libraries(threader-std PRIVATE
    # Qt5::Core
)

target_link_libraries(example1-messages
    # Qt5::Core
    -lthreader-std
)

target_link_libraries(example2-threads
    # Qt5::Core
    -lthreader-std
)

target_link_libraries(example3-pingpong
    # Qt5::Core
    -lthreader-std
    -lstdc++fs
)
