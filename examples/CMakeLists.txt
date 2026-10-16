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

# example01-messages

add_executable(example01-messages
    example01-messages/MainExample01.cpp
    )
target_compile_features(example01-messages PUBLIC cxx_std_17)

target_link_libraries(example01-messages
    threader-std
    )

#example02-threads
add_executable(example02-threads
    example02-threads/MainExample02.cpp
    example02-threads/ThreadTest.h example02-threads/ThreadTest.cpp
    )
target_compile_features(example02-threads PUBLIC cxx_std_17)

target_link_libraries(example02-threads
    threader-std
    )

# example03-pingpong
add_executable(example03-pingpong
    example03-pingpong/Example03Common.h
    example03-pingpong/MainExample03.cpp
    example03-pingpong/ThreadPing.h example03-pingpong/ThreadPing.cpp
    example03-pingpong/ThreadPong.h example03-pingpong/ThreadPong.cpp
    )
target_compile_features(example03-pingpong PUBLIC cxx_std_17)

target_link_libraries(example03-pingpong
    threader-std
    )

## example04-serial
add_executable(example04-serial
    example04-serial/Example04Common.h
    example04-serial/MainExample04.cpp
    example04-serial/ThreadPing.h example04-serial/ThreadPing.cpp
    example04-serial/ThreadPong.h example04-serial/ThreadPong.cpp
    )
target_compile_features(example04-serial PUBLIC cxx_std_17)

target_link_libraries(example04-serial
    threader-std
    )

# example05-gps
add_executable(example05-gps
    example05-simpleserver/Example05Common.h
    example05-simpleserver/MainExample05Gps.cpp
    example05-simpleserver/SettingsGpsEmulator.h example05-simpleserver/SettingsGpsEmulator.cpp
    example05-simpleserver/ThreadGpsEmulator.h example05-simpleserver/ThreadGpsEmulator.cpp
    example05-simpleserver/ThreadMainGps.h example05-simpleserver/ThreadMainGps.cpp
    )
target_compile_features(example05-gps PUBLIC cxx_std_17)

target_link_libraries(example05-gps
    threader-std
    )

# example05-simpleserver
add_executable(example05-timeserver
    example05-simpleserver/Example05Common.h
    example05-simpleserver/MainExample05TimeServer.cpp
    example05-simpleserver/SettingsGps.h example05-simpleserver/SettingsGps.cpp
    example05-simpleserver/SettingsTimeServer.h example05-simpleserver/SettingsTimeServer.cpp
    example05-simpleserver/ThreadConnectionServer.h example05-simpleserver/ThreadConnectionServer.cpp
    example05-simpleserver/ThreadGps.h example05-simpleserver/ThreadGps.cpp
    example05-simpleserver/ThreadListenConnections.h example05-simpleserver/ThreadListenConnections.cpp
    example05-simpleserver/ThreadMainTimeServer.h example05-simpleserver/ThreadMainTimeServer.cpp
    )
target_compile_features(example05-timeserver PUBLIC cxx_std_17)

target_link_libraries(example05-timeserver
    threader-std
    )

# example05-simpleclient
add_executable(example05-timeclient
    example05-simpleserver/Example05Common.h
    example05-simpleserver/MainExample05TimeClient.cpp
    example05-simpleserver/SettingsTimeServer.h example05-simpleserver/SettingsTimeServer.cpp
    example05-simpleserver/ThreadConnectionClient.h example05-simpleserver/ThreadConnectionClient.cpp
    example05-simpleserver/ThreadMainTimeClient.h example05-simpleserver/ThreadMainTimeClient.cpp
    )
target_compile_features(example05-timeclient PUBLIC cxx_std_17)

target_link_libraries(example05-timeclient
    threader-std
    )

if(UNIX)

# example06-devicelist
add_executable(example06-devicelist
    example06-udev/MainExample06DeviceList.cpp
    )

target_compile_features(example06-devicelist PUBLIC cxx_std_17)

target_link_libraries(example06-devicelist
    threader-std
    )

# example06-waitdevice
add_executable(example06-waitdevice
    example06-udev/MainExample06WaitDevice.cpp
    example06-udev/ThreadMainWaitDevice.h example06-udev/ThreadMainWaitDevice.cpp
    example06-udev/SettingsDevice.h example06-udev/SettingsDevice.cpp
    )

target_compile_features(example06-waitdevice PUBLIC cxx_std_17)

target_link_libraries(example06-waitdevice
    threader-std
    )

endif()

if(QT_ENABLE)
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

    # example07-1-qteventloop
    add_executable(example07-1-qteventloop
        example07-qteventloop/MainExample07-QtEventLoop.cpp
        example07-qteventloop/ThreadMainQtEventLoop.h example07-qteventloop/ThreadMainQtEventLoop.cpp
        )

    target_include_directories(example07-1-qteventloop PUBLIC
        ${CMAKE_SOURCE_DIR}/sources/libraries/threader.qt/include/threads
        )

    target_compile_features(example07-1-qteventloop PUBLIC cxx_std_17)

    target_link_libraries(example07-1-qteventloop
        threader-std
        threader-qt
        Qt5::Core
        )

    if(UNIX)
    #example07-2-udevlistener
    add_executable(example07-2-udevlistener
        example07-qteventloop/MainExample07-UdevListener.cpp
        example07-qteventloop/MainWindowUdevListener.cpp
        example07-qteventloop/MainWindowUdevListener.h
        example07-qteventloop/MainWindowUdevListener.ui
        )

    target_include_directories(example07-2-udevlistener PUBLIC
        ${CMAKE_SOURCE_DIR}/sources/libraries/threader.std/include/threads
        ${CMAKE_SOURCE_DIR}/sources/libraries/threader.std/include/utils
        ${CMAKE_SOURCE_DIR}/sources/libraries/threader.qt/include/threads
        ${CMAKE_SOURCE_DIR}/sources/libraries/threader.qt/include/utils
        )

    target_compile_features(example07-2-udevlistener PUBLIC cxx_std_17)

    target_link_libraries(example07-2-udevlistener
        threader-std
        threader-qt
        Qt5::Core
        Qt5::Widgets
        )
    endif()

    # #example11-messagebridge
    # add_executable(example11-messagebridge
    #     example11-messagebridge/MainExample11-MessageBridge.cpp
    #     example11-messagebridge/MainWindowMessageBridge.cpp
    #     example11-messagebridge/MainWindowMessageBridge.h
    #     example11-messagebridge/MainWindowMessageBridge.ui
    #     )

    # target_include_directories(example11-messagebridge PUBLIC
    #     ${CMAKE_SOURCE_DIR}/sources/libraries/threader.std/include/threads
    #     ${CMAKE_SOURCE_DIR}/sources/libraries/threader.std/include/utils
    #     ${CMAKE_SOURCE_DIR}/sources/libraries/threader.qt/include/threads
    #     ${CMAKE_SOURCE_DIR}/sources/libraries/threader.qt/include/utils
    #     )

    # target_compile_features(example11-messagebridge PUBLIC cxx_std_17)

    # target_link_libraries(example11-messagebridge PRIVATE
    #     threader-std
    #     threader-qt
    #     Qt5::Core
    #     Qt5::Widgets
    #     )

endif(QT_ENABLE)

if(UNIX)

#example08-udpsender
add_executable(example08-udpsender
    example08-udpfiletranser/Example08Common.h
    example08-udpfiletranser/MainExample08-UdpSender.cpp
    example08-udpfiletranser/ThreadMainUdpSender.cpp example08-udpfiletranser/ThreadMainUdpSender.h
    example08-udpfiletranser/ThreadUdpReceiver.cpp example08-udpfiletranser/ThreadUdpReceiver.h
    )

target_compile_features(example08-udpsender PUBLIC cxx_std_17)

target_link_libraries(example08-udpsender
    threader-std
    )

#example09-threadtimer
add_executable(example09-threadtimer
    example09-threadtimer/MainExample09-ThreadTimer.cpp
    example09-threadtimer/ThreadMainTimer.h example09-threadtimer/ThreadMainTimer.cpp
    )

target_compile_features(example09-threadtimer PUBLIC cxx_std_17)

target_link_libraries(example09-threadtimer
    threader-std
    )

#example10-filewatcher
add_executable(example10-filewatcher
    example10-filewatcher/MainExample10-FileWatcher.cpp
    example10-filewatcher/ThreadMainFileWatcher.h example10-filewatcher/ThreadMainFileWatcher.cpp
    example10-filewatcher/ThreadFileWatcher.h example10-filewatcher/ThreadFileWatcher.cpp
    )

target_compile_features(example10-filewatcher PUBLIC cxx_std_17)

target_link_libraries(example10-filewatcher
    threader-std
    )


#example12-checkdaemon
add_executable(example12-checkdaemon
    example12-checkdaemon/MainExample12-CheckDaemon.cpp
#    example11-checkdaemon/ThreadMainFileWatcher.h example10-filewatcher/ThreadMainFileWatcher.cpp
#    example11-checkdaemon/ThreadFileWatcher.h example10-filewatcher/ThreadFileWatcher.cpp
    )

find_package(PkgConfig REQUIRED)
pkg_check_modules(DBUS REQUIRED dbus-1)

target_compile_features(example12-checkdaemon PUBLIC cxx_std_17)

target_link_libraries(example12-checkdaemon
    threader-std
    ${DBUS_LIBRARIES}
    )

target_include_directories(example12-checkdaemon PRIVATE
    ${DBUS_INCLUDE_DIRS}
    )

endif()
