cmake_minimum_required(VERSION 2.8.11)
project(stored)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
# set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#Bring the headers, such as Student.h into the project
include_directories(include)

#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.c")

add_executable(hello ${SOURCES})