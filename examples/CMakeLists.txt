include_directories(
    ${CMAKE_SOURCE_DIR}/sources/libraries/threader.std/include/threads
    ${CMAKE_SOURCE_DIR}/sources/libraries/threader.std/include/utils
    )

if(QT_ENABLE)
    # Использовать запуск утилит Qt в автоматическом режиме
    # Автоматическая компиляция .ui файлов с помощью uic
    set(CMAKE_AUTOUIC ON)
    # Автоматический запуск Meta-Object Compiler с помощью moc
    set(CMAKE_AUTOMOC ON)
    # Автоматический запуск Resource Compiler с помощью rcc
    set(CMAKE_AUTORCC ON)
endif(QT_ENABLE)


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
target_compile_features(example2-threads PUBLIC cxx_std_17)

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
target_compile_features(example3-pingpong PUBLIC cxx_std_17)

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
target_compile_features(example4-serial PUBLIC cxx_std_17)

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
target_compile_features(example5-gps PUBLIC cxx_std_17)

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
target_compile_features(example5-timeserver PUBLIC cxx_std_17)

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
target_compile_features(example5-timeclient PUBLIC cxx_std_17)

target_link_libraries(example5-timeclient
    -lpthread
    threader-std
    )


# example6-devicelist
add_executable(example6-devicelist
    example6-udev/MainExample6DeviceList.cpp
    )

target_compile_features(example6-devicelist PUBLIC cxx_std_17)

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

target_compile_features(example6-waitdevice PUBLIC cxx_std_17)

target_link_libraries(example6-waitdevice
    pthread
    threader-std
    )

if(QT_ENABLE)
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

    # example7-qteventloop
    add_executable(example7-1-qteventloop
        example7-qteventloop/MainExample7-QtEventLoop.cpp
        example7-qteventloop/ThreadMainQtEventLoop.h example7-qteventloop/ThreadMainQtEventLoop.cpp
        )

    target_include_directories(example7-1-qteventloop PUBLIC
        ${CMAKE_SOURCE_DIR}/sources/libraries/threader.qt/include/threads
        )

    target_compile_features(example7-1-qteventloop PUBLIC cxx_std_17)

    target_link_libraries(example7-1-qteventloop
        pthread
        threader-std
        threader-qt
        Qt5::Core
        )

    #example7-udevlistener
    add_executable(example7-2-udevlistener
        example7-qteventloop/MainExample7-UdevListener.cpp
        example7-qteventloop/MainWindowUdevListener.cpp
        example7-qteventloop/MainWindowUdevListener.h
        example7-qteventloop/MainWindowUdevListener.ui
        )

    target_include_directories(example7-2-udevlistener PUBLIC
        ${CMAKE_SOURCE_DIR}/sources/libraries/threader.std/include/threads
        ${CMAKE_SOURCE_DIR}/sources/libraries/threader.std/include/utils
        ${CMAKE_SOURCE_DIR}/sources/libraries/threader.qt/include/threads
        ${CMAKE_SOURCE_DIR}/sources/libraries/threader.qt/include/utils
        )

    target_compile_features(example7-2-udevlistener PUBLIC cxx_std_17)

    target_link_libraries(example7-2-udevlistener
        pthread
        threader-std
        threader-qt
        Qt5::Core
        Qt5::Widgets
        )
endif(QT_ENABLE)

#example8-udpsender
add_executable(example8-udpsender
    example8-udpfiletranser/Example8Common.h
    example8-udpfiletranser/MainExample8-UdpSender.cpp
    example8-udpfiletranser/ThreadMainUdpSender.cpp example8-udpfiletranser/ThreadMainUdpSender.h
    example8-udpfiletranser/ThreadUdpReceiver.cpp example8-udpfiletranser/ThreadUdpReceiver.h
    )

target_compile_features(example8-udpsender PUBLIC cxx_std_17)

target_link_libraries(example8-udpsender
    pthread
    threader-std
    )

#example9-threadtimer
add_executable(example9-threadtimer
    example9-threadtimer/MainExample9-ThreadTimer.cpp
    example9-threadtimer/ThreadMainTimer.h example9-threadtimer/ThreadMainTimer.cpp
    )

target_compile_features(example9-threadtimer PUBLIC cxx_std_17)

target_link_libraries(example9-threadtimer
    pthread
    threader-std
    )
