cmake_minimum_required(VERSION 3.16)

project(threader-qt LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core)

set(THREADER_QT_SOURCES
    include/threads/DaemonApplicationQt.h
    include/threads/MessageBridge.h sources/threads/MessageBridge.cpp
    include/threads/ThreadQtEventLoop.h sources/threads/ThreadQtEventLoop.cpp
    include/threads/ThreadMainQt.h sources/threads/ThreadMainQt.cpp
)

if (UNIX)
    set(THREADER_QT_SOURCES "${THREADER_SOURCES}"
        ";include/utils/UdevListener.h;sources/utils/UdevListener.cpp"
    )
endif()

add_library(${PROJECT_NAME} SHARED ${THREADER_QT_SOURCES})


# add_library(${PROJECT_NAME} SHARED
#     include/threads/DaemonApplicationQt.h
#     include/threads/MessageBridge.h sources/threads/MessageBridge.cpp
#     include/threads/ThreadQtEventLoop.h sources/threads/ThreadQtEventLoop.cpp
#     include/threads/ThreadMainQt.h sources/threads/ThreadMainQt.cpp
#     include/utils/UdevListener.h sources/utils/UdevListener.cpp
#     )

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${VERSION}
    SOVERSION ${VER_MAJ}
    )

target_link_libraries(${PROJECT_NAME}
    pthread Qt5::Core threader-std
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/threads
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
    ${ROOT_PROJECT_DIR}/sources/libraries/threader.std/include/threads
    ${ROOT_PROJECT_DIR}/sources/libraries/threader.std/include/utils
)

include(${ROOT_PROJECT_DIR}/Common.cmake)

install(TARGETS ${PROJECT_NAME}
    LIBRARY
    DESTINATION "/usr/local/lib/threader.qt/"
    PERMISSIONS
    OWNER_READ OWNER_WRITE
    GROUP_READ
    WORLD_READ
    COMPONENT qt-lib
    )

install(FILES ${ROOT_PROJECT_DIR}/install/threader-qt.conf
    DESTINATION "/etc/ld.so.conf.d/"
    COMPONENT qt-lib
    )

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION "/usr/local/include/threader.qt"
    FILE_PERMISSIONS
    OWNER_READ OWNER_WRITE
    GROUP_READ
    WORLD_READ
    DIRECTORY_PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    COMPONENT qt-dev
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
    )

#include(CPack)
