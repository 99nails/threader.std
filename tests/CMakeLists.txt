SET(INCLUDE_PATH
    "${CMAKE_SOURCE_DIR}/sources/libraries/threader.std/include/threads"
    "${CMAKE_SOURCE_DIR}/sources/libraries/threader.std/include/utils"
)

include_directories(${INCLUDE_PATH})

if(TARGET GTest::gtest)
    set(GTEST_LIBS GTest::gtest GTest::gtest_main)
else()
    include(FetchContent)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    set(GTEST_LIBS gtest gtest_main)
endif()

add_executable(threader-std-tests
    test_queue_messages.cpp
    test_packet_factory_ascii.cpp
    test_ini_file.cpp
    test_std_thread_wrapper.cpp
    test_traffic_counter.cpp
    test_udev_device_info.cpp
    test_udev_enumerator.cpp
)

target_compile_features(threader-std-tests PUBLIC cxx_std_17)

target_link_libraries(threader-std-tests
    ${GTEST_LIBS}
    threader-std
)

add_test(NAME threader-std-tests COMMAND threader-std-tests)

set_tests_properties(threader-std-tests PROPERTIES
    ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_SOURCE_DIR}/lib.${OS_SUFFIX}"
)


if(QT_ENABLE)
    find_package(Qt5 REQUIRED COMPONENTS Core Test)

    set(CMAKE_AUTOMOC ON)

    add_executable(threader-qt-tests
        test_message_bridge.cpp
        )
    target_compile_features(threader-qt-tests PUBLIC cxx_std_17)
    target_include_directories(threader-qt-tests PRIVATE
        "${CMAKE_SOURCE_DIR}/sources/libraries/threader.qt/include/threads"
        "${CMAKE_SOURCE_DIR}/sources/libraries/threader.qt/include/utils"
        )
    target_link_libraries(threader-qt-tests
        ${GTEST_LIBS}
        threader-std
        threader-qt
        Qt5::Core
        Qt5::Test
        )
    add_test(NAME threader-qt-tests COMMAND threader-qt-tests)
    set_tests_properties(threader-qt-tests PROPERTIES
        ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_SOURCE_DIR}/lib.${OS_SUFFIX}"
        )
endif()

if(SSL_ENABLE)
    add_executable(threader-ssl-tests
        DISABLED_test_ssl_refcount.cpp
        test_ssl_handshake.cpp
    )
    target_compile_features(threader-ssl-tests PUBLIC cxx_std_17)
    target_include_directories(threader-ssl-tests PRIVATE
        "${CMAKE_SOURCE_DIR}/sources/libraries/threader.ssl/include/threads"
    )
    target_link_libraries(threader-ssl-tests
        ${GTEST_LIBS}
        threader-std
        threader-ssl
    )
    add_test(NAME threader-ssl-tests COMMAND threader-ssl-tests)
    set_tests_properties(threader-ssl-tests PROPERTIES
        ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_SOURCE_DIR}/lib.${OS_SUFFIX}"
    )
endif()

