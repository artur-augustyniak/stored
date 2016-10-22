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

# Required libs
find_package (Threads REQUIRED)
find_package(LibConfig REQUIRED)
find_package(Systemd REQUIRED)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

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