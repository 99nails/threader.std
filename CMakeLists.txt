cmake_minimum_required(VERSION 3.16)

set(VER_MAJ 1)
set(VER_MIN 1)
set(VER_PAT 0)
set(VER_BLD 1)

set(VERSION ${VER_MAJ}.${VER_MIN}.${VER_PAT})
set(VERSION4 ${VERSION}.${VER_BLD})

add_compile_definitions(THREADER_VERSION_STRING="${VERSION}")
add_compile_definitions(THREADER_VERSION4_STRING="${VERSION4}")

project(libthreader VERSION ${VERSION} LANGUAGES C CXX)

set_target_properties(${TARGET} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

set(OS_SUFFIX ${CMAKE_SYSTEM_NAME})
string(TOLOWER ${OS_SUFFIX} OS_SUFFIX)

option(QT_ENABLE "Build with Qt support" ON)
option(SSL_ENABLE "Build with SSL support" ON)

message("Target system name - [" ${OS_SUFFIX} "]")
message("Version [" ${VERSION4} "]")
message("Root project path: [" ${CMAKE_CURRENT_SOURCE_DIR} "]")

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(BUILD_SUFFIX release)
endif(CMAKE_BUILD_TYPE MATCHES Release)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(BUILD_SUFFIX debug)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

# Установка корневого каталога
set(ROOT_PROJECT_DIR ${CMAKE_SOURCE_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${BUILD_SUFFIX})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib.${OS_SUFFIX})
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

link_directories(${CMAKE_SOURCE_DIR}/lib.${OS_SUFFIX})

include_directories(${CMAKE_SOURCE_DIR}/sources/libraries/threader.std/include/threads)

add_subdirectory(sources/libraries/threader.std)

if (QT_ENABLE)
    add_subdirectory(sources/libraries/threader.qt)
endif()

if (SSL_ENABLE)
    add_subdirectory(sources/libraries/threader.ssl)
endif()

add_subdirectory(examples)

include(Common.cmake)

add_custom_target(doc-files SOURCES
    README.md
    TODO.md
    )

add_custom_target(install-files SOURCES
    install/threader-qt.conf
    install/threader-std.conf
    install/threader-ssl.conf
    install/threader.std/postinst
    install/threader.std/postrm
    install/threader.std/preinst
    install/threader.qt/postinst
    install/threader.qt/postrm
    install/threader.qt/preinst
    install/threader.ssl/postinst
    install/threader.ssl/postrm
    install/threader.ssl/preinst
#    bin/config/config.template.ini
    )

