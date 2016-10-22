cmake_minimum_required(VERSION 2.8.11)
project(stored)

set(ST_NAME ${PROJECT_NAME})
set(ST_VERSION 0.2)
add_definitions( -DST_NAME="${ST_NAME} ${ST_VERSION}")

# Custom cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Set the compiler and linker flags
set(GCC_COMPILE_FLAGS "-std=gnu11 -Wall -pedantic")
set(GCC_LINK_FLAGS "-lsystemd -lconfig -pthread")
set(TEST_GCC_COMPILE_FLAGS "-std=gnu11 -Wall -pedantic -g")
set(TEST_GCC_LINK_FLAGS "-pthread -lm")

# Set default install prefix
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "/" CACHE PATH "default install path" FORCE)
endif()

# Required libs
find_package (Threads REQUIRED)
find_package(LibConfig REQUIRED)
find_package(Systemd REQUIRED)

# Set the output folder where your program will be created
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CPACK_OUTPUT_FILE_PREFIX build)

# Sources
file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE TEST_SOURCES "test/misc/*.c")

# BUILD [make]
add_executable(stored ${SOURCES})
set_target_properties(stored PROPERTIES COMPILE_FLAGS ${GCC_COMPILE_FLAGS} LINK_FLAGS ${GCC_LINK_FLAGS})

# TESTING [make test || ctest]
enable_testing()
set(JSON_TEST test/misc/json_test.c src/util/json.c)
add_executable(json_test ${JSON_TEST})
set_target_properties(json_test PROPERTIES COMPILE_FLAGS ${TEST_GCC_COMPILE_FLAGS} LINK_FLAGS ${TEST_GCC_LINK_FLAGS})

add_test(NAME DummyTest COMMAND ./test/scripts/dummy.sh 0)
add_test(NAME JsonTest COMMAND ./build/json_test)

# PHONY TARGET
add_custom_target(valgrind COMMAND ./test/scripts/memory_leaks.sh)

# INSTALL
# xargs rm < install_manifest.txt
# $ cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr . && make all install
# Would configure the project, build all targets and install to the /usr prefix.
# The type (PATH) is not strictly necessary,
# but would cause the Qt based cmake-gui to present the directory chooser dialog.
install(TARGETS stored DESTINATION sbin COMPONENT binaries)
install(FILES etc/stored.cfg DESTINATION etc COMPONENT config)
install(FILES man/stored.8 DESTINATION usr/share/man/man8/)

# if(USE_SYSTEMD)
find_package(PkgConfig REQUIRED)
# check systemd
pkg_check_modules(SYSTEMD "libsystemd-login libsystemd-daemon" REQUIRED)
# find systemd unit dir
execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=systemdsystemunitdir systemd OUTPUT_VARIABLE SYSTEMD_SYSTEM_UNIT_DIR)
string(REGEX REPLACE "[ \t\n]+" \; SYSTEMD_SYSTEM_UNIT_DIR "${SYSTEMD_SYSTEM_UNIT_DIR}")
install(FILES systemd/stored.service DESTINATION ${SYSTEMD_SYSTEM_UNIT_DIR})
# endif(USE_SYSTEMD)

set(CPACK_GENERATOR "RPM")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_NAME "stored")
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Artur Augustyniak <artur@aaugustyniak.pl>")
set(CPACK_PACKAGE_VENDOR "Artur Augustyniak")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
include(CPack)
