cmake_minimum_required (VERSION 2.8)
project("disser")

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -O3")

#link_directories("${PROJECT_SOURCE_DIR}/pth-2.0.7/bin/lib")
find_library(pthread pthread HINTS "${PROJECT_SOURCE_DIR}/pth-2.0.7/bin/lib")
#include_directories("${PROJECT_SOURCE_DIR}/pth-2.0.7/bin/include")
#set(pthread_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/pth-2.0.7/bin/include")

#add_subdirectory("test1/pth")
#add_subdirectory("test1/pthread")
#add_subdirectory("test2")
add_subdirectory("test3")
