#####################
######## INIT #######
#####################
cmake_minimum_required(VERSION 3.0.0)
project(stored)
set (stored_VERSION_MAJOR 0)
set (stored_VERSION_MINOR 2)
set (stored_VERSION_PATCH 0)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/src/stored_config.h.in"
  "${PROJECT_BINARY_DIR}/src/stored_config.h"
  )

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

# cmake -D CMAKE_BUILD_TYPE=Debug .
# cmake .
# make VERBOSE=1
# Or you can add -DCMAKE_VERBOSE_MAKEFILE=ON to the cmake command for permanent verbose command output from the generated Makefiles.
# cmake -DCMAKE_VERBOSE_MAKEFILE=ON .
# make
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)

# Custom cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Set the compiler and linker flags
set(GCC_COMPILE_FLAGS "-std=gnu11 -Wall -pedantic -fstack-protector-all -D_FORTIFY_SOURCE=2 -fPIE ")
set(GCC_LINK_FLAGS "-lsystemd -lconfig -pthread -pie -Wl,-z,relro,-z,now")
set(TEST_GCC_COMPILE_FLAGS "-std=gnu11 -Wall -pedantic -g")
set(TEST_GCC_LINK_FLAGS "-pthread -lm -lconfig")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

# Set default install prefix
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/" CACHE PATH "default install path" FORCE)
endif ()

# Required libs
find_package(Threads REQUIRED)
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

#####################
####### BUILD #######
#####################
# make
add_executable(stored ${SOURCES})

set_target_properties(stored PROPERTIES COMPILE_FLAGS ${GCC_COMPILE_FLAGS} LINK_FLAGS ${GCC_LINK_FLAGS})
target_link_libraries(stored -lconfig -lsystemd)
#####################
###### TESTING ######
#####################
# make test || ctest
enable_testing()

set(JSON_TEST test/misc/json_test.c src/util/json.c src/util/common.c )
add_executable(json_test ${JSON_TEST})
set_target_properties(json_test PROPERTIES COMPILE_FLAGS ${TEST_GCC_COMPILE_FLAGS} LINK_FLAGS ${TEST_GCC_LINK_FLAGS})
target_link_libraries(json_test -lconfig -lm)
add_test(NAME JsonTest COMMAND ./build/json_test)

set(CONFIGURE_TEST test/misc/configure_test.c src/util/configure.c src/util/common.c )
add_executable(configure_test ${CONFIGURE_TEST})
target_link_libraries(configure_test -lconfig)
set_target_properties(configure_test PROPERTIES COMPILE_FLAGS ${TEST_GCC_COMPILE_FLAGS} LINK_FLAGS ${TEST_GCC_LINK_FLAGS})
add_test(NAME ConfigureTest COMMAND ./build/configure_test)

set(LOGGER_TEST test/misc/logger_test.c src/util/logger.c src/util/sds.c src/util/common.c)
add_executable(logger_test ${LOGGER_TEST})
set_target_properties(logger_test PROPERTIES COMPILE_FLAGS ${TEST_GCC_COMPILE_FLAGS} LINK_FLAGS ${TEST_GCC_LINK_FLAGS})
add_test(NAME LoggerTest COMMAND ./build/logger_test)

set(MTAB_TEST
    test/misc/mtab_checker_test.c
    src/mtab_checker.c
    src/util/configure.c
    src/util/logger.c
    src/util/sds.c
    src/util/json.c
    src/util/common.c
)
add_executable(mtab_test ${MTAB_TEST})
target_link_libraries(mtab_test -lconfig)
set_target_properties(mtab_test PROPERTIES COMPILE_FLAGS ${TEST_GCC_COMPILE_FLAGS} LINK_FLAGS ${TEST_GCC_LINK_FLAGS})
add_test(NAME MtabTest COMMAND ./build/mtab_test)

set(SRV_TEST
    test/misc/srv_test.c
    src/util/srv.c
    src/util/configure.c
    src/util/common.c
)
add_executable(srv_test ${SRV_TEST})
target_link_libraries(srv_test -lconfig)
set_target_properties(srv_test PROPERTIES COMPILE_FLAGS ${TEST_GCC_COMPILE_FLAGS} LINK_FLAGS ${TEST_GCC_LINK_FLAGS})
add_test(NAME SrvTest COMMAND ./build/srv_test)

#####################
####### PHONY #######
#####################
add_custom_target(uninstall COMMAND xargs rm < ./install_manifest.txt)

add_custom_target(valgrind_concurrency COMMAND ./test/scripts/concurrency_bugs.sh)
add_custom_target(valgrind_memory COMMAND ./test/scripts/memory_leaks.sh)
add_custom_target(stress COMMAND ./test/scripts/stress_test.sh 40 0)
add_custom_target(flawfind COMMAND ./test/scripts/flawfind.sh)

#####################
###### INSTALL ######
#####################
# $ cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr . && make all install
# Would configure the project, build all targets and install to the /usr prefix.
# The type (PATH) is not strictly necessary,
# but would cause the Qt based cmake-gui to present the directory chooser dialog.
install(TARGETS stored DESTINATION usr/sbin COMPONENT binaries)
install(FILES etc/stored.cfg DESTINATION etc/stored COMPONENT config)
install(FILES man/stored.8 DESTINATION usr/share/man/man8)
find_package(PkgConfig REQUIRED)
#pkg_check_modules(SYSTEMD "libsystemd-login libsystemd-daemon" REQUIRED)
execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=systemdsystemunitdir systemd OUTPUT_VARIABLE SYSTEMD_SYSTEM_UNIT_DIR)
string(REGEX REPLACE "[ \t\n]+" \; SYSTEMD_SYSTEM_UNIT_DIR "${SYSTEMD_SYSTEM_UNIT_DIR}")
install(FILES systemd/stored.service DESTINATION usr/lib/systemd/system)

#####################
##### PACKAGING #####
#####################
set(CPACK_GENERATOR "RPM;DEB")
set(CPACK_INSTALL_PREFIX /)
# set(CPACK_SET_DESTDIR true)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_DESCRIPTION "Mount point monitoring daemon.")
set(CPACK_PACKAGE_CONTACT "Artur Augustyniak artur@aaugustyniak.pl")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_VERSION_MAJOR ${stored_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${stored_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${stored_VERSION_PATCH})
#RPM SPECIFIC
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST
    /usr
    /usr/bin
    /usr/sbin
    /usr/share
    /usr/share/man
    /usr/share/man/man8
    /usr/lib
    /usr/lib/systemd
    /usr/lib/systemd/system
    /etc
)
#DEB SPECIFIC
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Artur Augustyniak <artur@aaugustyniak.pl>")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/Debian/postinst")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
include(CPack)