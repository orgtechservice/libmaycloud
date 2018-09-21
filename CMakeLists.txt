
# Название проекта, минимально требуемая версия CMake, стандарт C++ (включить если понадобится)
cmake_minimum_required(VERSION 3.1)
project(libmaycloud)
#set(CMAKE_CXX_STANDARD 11)

# Основные переменные окружения, задающие флаги компилятора и префикс для установки (также применяется к deb-пакету)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_INSTALL_PREFIX "/usr/local")

# Проверим наличие зависимостей
find_package(EXPAT)
find_package(SQLITE3)
find_package(UDNS)

# Исходники и заголовочные файлы проекта
FILE(GLOB LIBMAYCLOUD_SOURCE src/*.cpp)
FILE(GLOB LIBMAYCLOUD_HEADERS src/headers/maycloud/*.h)

# Каталоги для добавления в список включения (опция -I компилятора)
include_directories(src/headers)

# Цель, создающая статическую библиотеку libmaycloud.a
add_library(maycloud STATIC ${LIBMAYCLOUD_SOURCE})

# Установка компонентов — библиотеки и заголовочных файлов
INSTALL(TARGETS maycloud LIBRARY DESTINATION lib ARCHIVE DESTINATION lib COMPONENT maycloud)
INSTALL(FILES ${LIBMAYCLOUD_HEADERS} DESTINATION include/maycloud COMPONENT dev)

# Реализация кастомной цели distclean
ADD_CUSTOM_TARGET (distclean @echo cleaning for source distribution)
set(DISTCLEANED CMakeCache.txt CMakeFiles cmake_install.cmake install_manifest_maycloud.txt install_manifest_dev.txt Makefile cmake_deb_package_dev cmake_deb_package_maycloud *.a *.deb)
ADD_CUSTOM_COMMAND (
	DEPENDS clean
	COMMENT "distribution clean"
	COMMAND rm
	ARGS -Rf ${DISTCLEANED}
	TARGET  distclean
)

# Компоненты, которые пакуем в deb-пакеты при помощи цели «deb»
set(DEB_PACKAGE_COMPONENTS "maycloud" "dev")

# Заготовка control-файла для libmaycloud
set(DEB_PACKAGE_maycloud_NAME "${CMAKE_PROJECT_NAME}")
set(DEB_PACKAGE_maycloud_VERSION "0.0.1")
set(DEB_PACKAGE_maycloud_SECTION "utils")
set(DEB_PACKAGE_maycloud_DESRCIPTION "MayCloud core library")
set(DEB_PACKAGE_maycloud_MAINTAINER "Orgtechservice, Ltd. <dev@mkpnet.ru>")
set(DEB_PACKAGE_maycloud_ARCH "amd64")
set(DEB_PACKAGE_maycloud_DEPENDS "libudns0, libsqlite3-0, libexpat1, mariadb-client | mysql-client")

# Заготовка control-файла для libmaycloud-dev
set(DEB_PACKAGE_dev_NAME "${CMAKE_PROJECT_NAME}-dev")
set(DEB_PACKAGE_dev_VERSION "0.0.1")
set(DEB_PACKAGE_dev_SECTION "development")
set(DEB_PACKAGE_dev_DESRCIPTION "MayCloud core library (development files)")
set(DEB_PACKAGE_dev_MAINTAINER "Orgtechservice, Ltd. <dev@mkpnet.ru>")
set(DEB_PACKAGE_dev_ARCH "amd64")
set(DEB_PACKAGE_dev_DEPENDS "${CMAKE_PROJECT_NAME}, libudns-dev, libsqlite3-dev, libexpat1-dev, libmysqlclient-dev | libperconaserverclient-dev | libmariadb-client-lgpl-dev | libmariadbclient-dev")

# Кастомная цель для формирования deb-пакетов
add_custom_target(deb DEPENDS deb_package)

# Костыль, куда ж без них в нашем нелёгком деле
add_custom_target(dev DEPENDS maycloud)

# Макрос, реализующий упаковку в deb-пакет
include(cmake/deb_packaging.cmake)
