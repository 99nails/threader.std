include_directories(
    ${CMAKE_SOURCE_DIR}/sources/libraries/threader.std/include/threads
    ${CMAKE_SOURCE_DIR}/sources/libraries/threader.std/include/utils
)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# example1-messages

add_executable(example1-messages
    example1-messages/MainExample1.cpp
)
target_compile_features(example1-messages PUBLIC cxx_std_17)

target_link_libraries(example1-messages
    pthread
#    Qt5::Core
    threader-std
)

#example2-threads
add_executable(example2-threads
    example2-threads/MainExample2.cpp
    example2-threads/ThreadTest.h example2-threads/ThreadTest.cpp
)
target_compile_features(example2-threads PUBLIC cxx_std_11)

target_link_libraries(example2-threads
    -lpthread
#        Qt5::Core
    threader-std
)

# example3-pingpong
add_executable(example3-pingpong
    example3-pingpong/Example3Common.h
    example3-pingpong/MainExample3.cpp
    example3-pingpong/ThreadPing.h example3-pingpong/ThreadPing.cpp
    example3-pingpong/ThreadPong.h example3-pingpong/ThreadPong.cpp
)
target_compile_features(example3-pingpong PUBLIC cxx_std_11)

target_link_libraries(example3-pingpong
    -lpthread
    threader-std
)

## example4-serial
add_executable(example4-serial
   example4-serial/Example4Common.h
   example4-serial/MainExample4.cpp
   example4-serial/ThreadPing.h example4-serial/ThreadPing.cpp
   example4-serial/ThreadPong.h example4-serial/ThreadPong.cpp
)
target_compile_features(example4-serial PUBLIC cxx_std_11)

target_link_libraries(example4-serial
   -lpthread
   threader-std
)

# example5-gps
add_executable(example5-gps
   example5-simpleserver/Example5Common.h
   example5-simpleserver/MainExample5Gps.cpp
   example5-simpleserver/SettingsGpsEmulator.h example5-simpleserver/SettingsGpsEmulator.cpp
   example5-simpleserver/ThreadGpsEmulator.h example5-simpleserver/ThreadGpsEmulator.cpp
   example5-simpleserver/ThreadMainGps.h example5-simpleserver/ThreadMainGps.cpp
)
target_compile_features(example5-gps PUBLIC cxx_std_11)

target_link_libraries(example5-gps
  pthread
  threader-std
)

# example5-simpleserver
add_executable(example5-timeserver
   example5-simpleserver/Example5Common.h
   example5-simpleserver/MainExample5TimeServer.cpp
   example5-simpleserver/SettingsGps.h example5-simpleserver/SettingsGps.cpp
   example5-simpleserver/SettingsTimeServer.h example5-simpleserver/SettingsTimeServer.cpp
   example5-simpleserver/ThreadConnectionServer.h example5-simpleserver/ThreadConnectionServer.cpp
   example5-simpleserver/ThreadGps.h example5-simpleserver/ThreadGps.cpp
   example5-simpleserver/ThreadListenConnections.h example5-simpleserver/ThreadListenConnections.cpp
   example5-simpleserver/ThreadMainTimeServer.h example5-simpleserver/ThreadMainTimeServer.cpp
)
target_compile_features(example5-timeserver PUBLIC cxx_std_11)

target_link_libraries(example5-timeserver
   pthread
   threader-std
)

# example5-simpleclient
add_executable(example5-timeclient
   example5-simpleserver/Example5Common.h
   example5-simpleserver/MainExample5TimeClient.cpp
   example5-simpleserver/SettingsTimeServer.h example5-simpleserver/SettingsTimeServer.cpp
   example5-simpleserver/ThreadConnectionClient.h example5-simpleserver/ThreadConnectionClient.cpp
   example5-simpleserver/ThreadMainTimeClient.h example5-simpleserver/ThreadMainTimeClient.cpp
)
target_compile_features(example5-timeclient PUBLIC cxx_std_11)

target_link_libraries(example5-timeclient
  -lpthread
  threader-std
)


# example6-devicelist
add_executable(example6-devicelist
   example6-udev/MainExample6DeviceList.cpp
   )

target_compile_features(example6-devicelist PUBLIC cxx_std_11)

target_link_libraries(example6-devicelist
  pthread
  threader-std
)

# example6-waitdevice
add_executable(example6-waitdevice
   example6-udev/MainExample6WaitDevice.cpp
   example6-udev/ThreadMainWaitDevice.h example6-udev/ThreadMainWaitDevice.cpp
   example6-udev/SettingsDevice.h example6-udev/SettingsDevice.cpp
   )

target_compile_features(example6-waitdevice PUBLIC cxx_std_11)

target_link_libraries(example6-waitdevice
  pthread
  threader-std
)
