
set(CPACK_DEB_COMPONENT_INSTALL ON)

set(CPACK_COMPONENTS_ALL std-lib std-dev qt-lib qt-dev)
set(CPACK_GERATOR "DEB")

set(CPACK_PACKAGE_VERSION_MAJOR ${VER_MAJ})
set(CPACK_PACKAGE_VERSION_MINOR ${VER_MIN})
set(CPACK_PACKAGE_VERSION_PATCH ${VER_PAT})
set(CPACK_PACKAGE_CONTACT "Maxim Mazitov <99nails@gmail.com>")
set(CPACK_PACKAGE_NAME "libthreader")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Maxim Mazitov <99nails@gmail.com>")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Multithreaded applications framework library")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")

set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_STRIP_FILES ON)

set(CPACK_COMPONENT_STD-LIB_DESCRIPTION "Multithreaded applications framework library")
set(CPACK_DEBIAN_STD-LIB_PACKAGE_DEPENDS "udev")
set(CPACK_DEBIAN_STD-LIB_PACKAGE_CONTROL_EXTRA
    "${CMAKE_SOURCE_DIR}/install/threader.std/preinst"
    "${CMAKE_SOURCE_DIR}/install/threader.std/postinst"
    "${CMAKE_SOURCE_DIR}/install/threader.std/postrm"
    )

set(CPACK_COMPONENT_STD-DEV_DESCRIPTION "Headers for multithreaded applications framework library")
set(CPACK_DEBIAN_STD-DEV_PACKAGE_DEPENDS "libudev-dev, ${CPACK_PACKAGE_NAME}-std-lib")

set(CPACK_COMPONENT_QT-LIB_DESCRIPTION "Qt support for multithreaded applications framework library")
set(CPACK_DEBIAN_QT-LIB_PACKAGE_DEPENDS "${CPACK_PACKAGE_NAME}-std-lib")
set(CPACK_DEBIAN_QT-LIB_PACKAGE_CONTROL_EXTRA
    "${CMAKE_SOURCE_DIR}/install/threader.qt/preinst"
    "${CMAKE_SOURCE_DIR}/install/threader.qt/postinst"
    "${CMAKE_SOURCE_DIR}/install/threader.qt/postrm"
    )

set(CPACK_COMPONENT_QT-DEV_DESCRIPTION "Headers for Qt support for multithreaded applications framework library")
set(CPACK_DEBIAN_QT-DEV_PACKAGE_DEPENDS "${CPACK_PACKAGE_NAME}-qt-lib")

include(CPack)
include(GNUInstallDirs)
