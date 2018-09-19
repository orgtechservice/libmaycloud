cmake_minimum_required(VERSION 2.8)
project(libmaycloud)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall")

set(LIBMAYCLOUD_SOURCE src/asn1reader.cpp src/asyncxmlstream.cpp src/easylist.cpp src/hasher.cpp src/mysqlconnect.cpp src/randomstring.cpp src/tempstring.cpp src/asyncagent.cpp src/attagparser.cpp src/easymysql.cpp src/heavyhash.cpp src/mysql.cpp src/snmpbuilder.cpp src/xmlparser.cpp src/asyncdns.cpp src/configurer.cpp src/easyresultset.cpp src/iputils.cpp src/netdaemon.cpp src/snmpparser.cpp src/xml_tag.cpp src/asyncobject.cpp src/controlagent.cpp src/easyrow.cpp src/ipv4.cpp src/object.cpp src/sqlite3.cpp src/xml_types.cpp src/asyncserver.cpp src/controlstream.cpp src/easyvector.cpp src/logger.cpp src/oid.cpp src/stack.cpp src/xmlwriter.cpp src/asyncstream.cpp src/debug.cpp src/error.cpp src/macaddress64.cpp src/processmanager.cpp src/tagbuilder.cpp src/asyncudpserver.cpp src/easylib.cpp src/fstream.cpp src/macaddress.cpp src/queue.cpp src/taghelper.cpp)

set(LIBMAYCLOUD_HEADERS src/headers/maycloud/asn1reader.h src/headers/maycloud/debug.h src/headers/maycloud/logger.h src/headers/maycloud/snmpbuilder.h src/headers/maycloud/asyncagent.h src/headers/maycloud/easylib.h src/headers/maycloud/macaddress64.h src/headers/maycloud/snmpparser.h src/headers/maycloud/asyncdns.h src/headers/maycloud/easylist.h src/headers/maycloud/macaddress.h src/headers/maycloud/sqlite3.h src/headers/maycloud/asyncobject.h src/headers/maycloud/easymysql.h src/headers/maycloud/map.h src/headers/maycloud/stack.h src/headers/maycloud/asyncserver.h src/headers/maycloud/easyresultset.h src/headers/maycloud/mysqlconnect.h src/headers/maycloud/stream.h src/headers/maycloud/asyncstream.h src/headers/maycloud/easyrow.h src/headers/maycloud/mysql.h src/headers/maycloud/tagbuilder.h src/headers/maycloud/asyncudpserver.h src/headers/maycloud/easyvector.h src/headers/maycloud/netdaemon.h src/headers/maycloud/taghelper.h src/headers/maycloud/asyncxmlstream.h src/headers/maycloud/error.h src/headers/maycloud/object.h src/headers/maycloud/tempstring.h src/headers/maycloud/attagparser.h src/headers/maycloud/fstream.h src/headers/maycloud/oid.h src/headers/maycloud/xmlparser.h src/headers/maycloud/config.h src/headers/maycloud/hasher.h src/headers/maycloud/processmanager.h src/headers/maycloud/xml_tag.h src/headers/maycloud/configurer.h src/headers/maycloud/heavyhash.h src/headers/maycloud/queue.h src/headers/maycloud/xml_types.h src/headers/maycloud/controlagent.h src/headers/maycloud/iputils.h src/headers/maycloud/randomstring.h src/headers/maycloud/xmlwriter.h src/headers/maycloud/controlstream.h src/headers/maycloud/ipv4.h src/headers/maycloud/safemap.h)

include_directories(/usr/include src/headers)

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
