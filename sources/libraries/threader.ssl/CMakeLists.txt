cmake_minimum_required(VERSION 3.16)

find_package(OpenSSL REQUIRED)

project(threader-ssl LANGUAGES CXX VERSION ${VERSION})

set(THREADER_SSL_SOURCES
    include/threads/HandlerSslSocket.h sources/threads/HandlerSslSocket.cpp
)

if (WIN32)
endif()

if (UNIX)
endif()

add_library(${PROJECT_NAME} SHARED ${THREADER_SSL_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/threads
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
    ${ROOT_PROJECT_DIR}/sources/libraries/threader.std/include/threads
    ${ROOT_PROJECT_DIR}/sources/libraries/threader.std/include/utils
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${VERSION} SOVERSION ${VER_MAJ})

if (WIN32)
    target_link_libraries(${PROJECT_NAME}
        threader-std
        OpenSSL::SSL
        OpenSSL::Crypto
    )
endif()

if (UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(_OPENSSL REQUIRED openssl)
    target_link_libraries(${PROJECT_NAME}
        threader-std
        ${_OPENSSL_LIBRARIES}
    )
endif()

set(LIBRARY_INSTALL_PATH "/usr/local/lib/threader.ssl/")
set(HEADERS_INSTALL_PATH "/usr/local/include/threader.ssl")
set(CONFIG_INSTALL_PATH  "/etc/ld.so.conf.d/")

install(TARGETS ${PROJECT_NAME}
    LIBRARY
    DESTINATION ${LIBRARY_INSTALL_PATH}
    PERMISSIONS
    OWNER_READ OWNER_WRITE
    GROUP_READ
    WORLD_READ
    COMPONENT ssl-lib
)

install(FILES ${ROOT_PROJECT_DIR}/install/threader-ssl.conf
    DESTINATION ${CONFIG_INSTALL_PATH}
    COMPONENT ssl-lib
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION ${HEADERS_INSTALL_PATH}
    FILE_PERMISSIONS
    OWNER_READ OWNER_WRITE
    GROUP_READ
    WORLD_READ
    DIRECTORY_PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    COMPONENT ssl-dev
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
)

include(CPack)
