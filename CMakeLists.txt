cmake_minimum_required(VERSION 2.8)
project(libmaycloud)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall")

FILE(GLOB LIBMAYCLOUD_SOURCE src/*.cpp)
FILE(GLOB LIBMAYCLOUD_HEADERS src/headers/maycloud/*.h)

include_directories(src/headers)

add_library(maycloud STATIC ${LIBMAYCLOUD_SOURCE})

INSTALL(TARGETS maycloud LIBRARY DESTINATION lib ARCHIVE DESTINATION lib COMPONENT maycloud)

INSTALL(FILES ${LIBMAYCLOUD_HEADERS} DESTINATION include/maycloud COMPONENT dev)

ADD_CUSTOM_TARGET (distclean @echo cleaning for source distribution)
SET(DISTCLEANED CMakeCache.txt CMakeFiles cmake_install.cmake install_manifest_maycloud.txt install_manifest_dev.txt Makefile cmake_deb_package_dev cmake_deb_package_maycloud *.a *.deb)

ADD_CUSTOM_COMMAND (
	DEPENDS clean
	COMMENT "distribution clean"
	COMMAND rm
	ARGS -Rf ${DISTCLEANED}
	TARGET  distclean
)

set(DEB_PACKAGE_COMPONENTS "maycloud" "dev")

set(DEB_PACKAGE_maycloud_NAME "${CMAKE_PROJECT_NAME}")
set(DEB_PACKAGE_maycloud_VERSION "0.0.1")
set(DEB_PACKAGE_maycloud_SECTION "utils")
set(DEB_PACKAGE_maycloud_DESRCIPTION "MayCloud core library")
set(DEB_PACKAGE_maycloud_MAINTAINER "Orgtechservice, Ltd. <dev@mkpnet.ru>")
set(DEB_PACKAGE_maycloud_DEPENDS "libudns0, libsqlite3-0")

set(DEB_PACKAGE_dev_NAME "${CMAKE_PROJECT_NAME}-dev")
set(DEB_PACKAGE_dev_VERSION "0.0.1")
set(DEB_PACKAGE_dev_SECTION "development")
set(DEB_PACKAGE_dev_DESRCIPTION "MayCloud core library (development files)")
set(DEB_PACKAGE_dev_MAINTAINER "Orgtechservice, Ltd. <dev@mkpnet.ru>")
set(DEB_PACKAGE_dev_ARCH "amd64")
set(DEB_PACKAGE_dev_DEPENDS "${CMAKE_PROJECT_NAME}, libudns-dev, libsqlite3-dev")

add_custom_target(deb
	DEPENDS deb_package
)

# Костыль
add_custom_target(dev
	DEPENDS maycloud
)

include(cmake/deb_packaging.cmake)
