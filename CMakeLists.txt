cmake_minimum_required(VERSION 3.16)

set(VER_MAJ 0)
set(VER_MIN 0)
set(VER_PAT 5)
set(VER_BLD 0)

set(VERSION ${VER_MAJ}.${VER_MIN}.${VER_PAT})
set(VERSION4 ${VERSION}.${VER_BLD})

add_compile_definitions(THREADER_VERSION_STRING="${VERSION}")
add_compile_definitions(THREADER_VERSION4_STRING="${VERSION4}")

project(threader-std VERSION ${VERSION} LANGUAGES C CXX)

set_target_properties(${TARGET} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

SET(OS_SUFFIX ${CMAKE_SYSTEM_NAME})
string(TOLOWER ${OS_SUFFIX} OS_SUFFIX)

message("Target system name - [" ${OS_SUFFIX} "]")
message("Version [" ${VERSION4} "]")
message("Root project path: ${CMAKE_CURRENT_SOURCE_DIR}")

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(BUILD_SUFFIX release)
endif(CMAKE_BUILD_TYPE MATCHES Release)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(BUILD_SUFFIX debug)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${BUILD_SUFFIX})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib.${OS_SUFFIX})
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

link_directories(${CMAKE_SOURCE_DIR}/lib.${OS_SUFFIX})

include_directories(${CMAKE_SOURCE_DIR}/sources/libraries/threader.std/include/threads)

add_subdirectory(sources/libraries/threader.std)


SET(CMAKE_SKIP_BUILD_RPATH TRUE)
SET(CMAKE_SKIP_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH ${PROJECT_NAME} "/usr/local/lib/threader.std/")

install(TARGETS ${PROJECT_NAME}
            LIBRARY
                DESTINATION /usr/local/lib/threader.std/
                PERMISSIONS
                    OWNER_READ OWNER_WRITE
                    GROUP_READ
                    WORLD_READ
                COMPONENT lib
)

install(FILES ${PROJECT_SOURCE_DIR}/install/threader-std.conf
        DESTINATION /etc/ld.so.conf.d/
        COMPONENT lib
        )

install(DIRECTORY ${PROJECT_SOURCE_DIR}/sources/libraries/threader.std/include/
            DESTINATION /usr/local/include/threader.std
            FILE_PERMISSIONS
            OWNER_READ OWNER_WRITE
            GROUP_READ
            WORLD_READ
            DIRECTORY_PERMISSIONS
            OWNER_READ OWNER_WRITE OWNER_EXECUTE
            GROUP_READ GROUP_EXECUTE
            WORLD_READ WORLD_EXECUTE
            COMPONENT dev
            FILES_MATCHING
            PATTERN "*.h"
            PATTERN "*.hpp"
)


SET(CPACK_GERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Maxim Mazitov <99nails@gmail.com>")
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Multithreaded applications framework library")
set(CPACK_PACKAGE_NAME "libthreaderstd")
SET(CPACK_DEBIAN_PACKAGE_MAJOR ${VER_MAJ})
SET(CPACK_DEBIAN_PACKAGE_MINOR ${VER_MIN})
SET(CPACK_DEBIAN_PACKAGE_PATCH ${VER_PAT})
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_STRIP_FILES ON)
set(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS "${CPACK_PACKAGE_NAME}-lib")
include(CPack)
