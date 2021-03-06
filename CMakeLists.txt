
# Название проекта, минимально требуемая версия CMake, стандарт C++ (включить если понадобится)
cmake_minimum_required(VERSION 3.1)
project(libmaycloud)
set(CMAKE_CXX_STANDARD 11)
set(VERSION "0.0.1")

# Основные переменные окружения, задающие флаги компилятора и префикс для установки (также применяется к deb-пакету)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -pedantic -ffunction-sections")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -ffunction-sections")
set(CMAKE_INSTALL_PREFIX "/usr/local")

# Проверим наличие зависимостей
find_package(EXPAT REQUIRED)
find_package(SQLITE3 REQUIRED)
find_package(UDNS REQUIRED)
find_library(MYSQL_LIBRARY NAMES mysqlclient perconaserverclient mariadbclient)

# Исходники и заголовочные файлы проекта
FILE(GLOB LIBMAYCLOUD_SOURCE src/*.cpp)
FILE(GLOB LIBMAYCLOUD_HEADERS src/headers/maycloud/*.h src/headers/maycloud/*.hpp)

IF(NOT MYSQL_LIBRARY)
	message("\n==============================================\n-- WARNING: Building without MySQL support! --\n==============================================\n")
	LIST(FILTER LIBMAYCLOUD_SOURCE EXCLUDE REGEX "(mysql|easymysql|easyresultset)\\.cpp$")
	LIST(FILTER LIBMAYCLOUD_HEADERS EXCLUDE REGEX "(mysql|easymysql|easyresultset)\\.h$")
else()
	message("-- Building with MySQL support")
	set(LIBMAYCLOUD_MYSQL "yes")
endif()

if(DISABLE_HTTP)
	message("\n==============================================\n-- WARNING: Building without HTTP support! --\n==============================================\n")
	LIST(FILTER LIBMAYCLOUD_SOURCE EXCLUDE REGEX "(asyncwebserver|http[a-z]+|html[a-z]+)\\.cpp$")
	LIST(FILTER LIBMAYCLOUD_HEADERS EXCLUDE REGEX "(asyncwebserver|http[a-z]+|html[a-z]+|inja|json)\\.(h|hpp)$")
else()
	message("-- Building with HTTP support")
	set(LIBMAYCLOUD_HTTP "yes")
endif()

# Файл активной конфигурации библиотеки
configure_file(${PROJECT_SOURCE_DIR}/src/headers/maycloud/config.h.in ${PROJECT_SOURCE_DIR}/src/headers/maycloud/config.h @ONLY)

# Каталоги для добавления в список включения (опция -I компилятора)
include_directories(${PROJECT_SOURCE_DIR}/src/headers)

# Цель, создающая статическую библиотеку libmaycloud.a
add_library(maycloud STATIC ${LIBMAYCLOUD_SOURCE})

# Установка компонентов — библиотеки и заголовочных файлов
INSTALL(TARGETS maycloud LIBRARY DESTINATION lib ARCHIVE DESTINATION lib COMPONENT maycloud)
INSTALL(FILES ${LIBMAYCLOUD_HEADERS} DESTINATION include/maycloud COMPONENT dev)
INSTALL(FILES ${PROJECT_SOURCE_DIR}/src/headers/maycloud/config.h DESTINATION include/maycloud COMPONENT dev)

# Реализация кастомной цели distclean
ADD_CUSTOM_TARGET (distclean @echo cleaning for source distribution)
set(DISTCLEANED CMakeCache.txt CMakeFiles _CPack_Packages install_manifest.txt cmake_install.cmake install_manifest_maycloud.txt install_manifest_dev.txt Makefile cmake_deb_package_dev cmake_deb_package_maycloud *.a *.deb *.cmake "src/headers/maycloud/config.h")
ADD_CUSTOM_COMMAND (
	DEPENDS clean
	COMMENT "distribution clean"
	COMMAND rm
	ARGS -Rf ${DISTCLEANED}
	TARGET  distclean
)

# Определяем архитектуру машины
if("${CMAKE_HOST_SYSTEM_PROCESSOR}" MATCHES "unknown")
	message("-- Building for ARM")
	set(PKGARCH "armhf")
elseif("${CMAKE_HOST_SYSTEM_PROCESSOR}" MATCHES "arm")
	message("-- Building for ARM")
	set(PKGARCH "armhf")
else()
	message("-- Building for amd64")
	set(PKGARCH "amd64")
endif()

set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "libmaycloud")
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Ilya Averkov")
set(CPACK_PACKAGE_VENDOR "Orgtechservice, Ltd.")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libudns0, libsqlite3-0, libexpat1")
set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")
include(CPack)
