cmake_minimum_required(VERSION 2.8.11)
project(stored)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Set the compiler and linker flags
set(GCC_COMPILE_FLAGS "-std=gnu11 -Wall -pedantic -g")
set(GCC_LINK_FLAGS "-lsystemd -lconfig -pthread")

# Append to the existing compiler and linker flags
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${GCC_COMPILE_FLAGS}" )
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_LINK_FLAGS}" )

# Bring the headers,
# include_directories(include)

# Can manually add the sources using the set command as follows:
# set(SOURCES src/mainapp.cpp src/Student.cpp)

# However, the file(GLOB... GLOB_RECURSE) allows for wildcard additions:
file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(stored ${SOURCES})