cmake_minimum_required(VERSION 3.16)

project(threader-qt LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core)

add_library(${PROJECT_NAME} SHARED
    include/threads/DaemonApplicationQt.h
    include/threads/ThreadQtEventLoop.h sources/threads/ThreadQtEventLoop.cpp
    include/threads/ThreadMainQt.h sources/threads/ThreadMainQt.cpp
    )

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${VERSION}
    SOVERSION ${VER_MAJ}
    )

set(LINK_LIBRARIES pthread Qt5::Core threader-std)

target_link_libraries(${PROJECT_NAME}
    ${LINK_LIBRARIES}
)

message("Project root dir ${ROOT_PROJECT_DIR}")

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/threads
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils

    ${ROOT_PROJECT_DIR}/sources/libraries/threader.std/include/threads
    ${ROOT_PROJECT_DIR}/sources/libraries/threader.std/include/utils
)


