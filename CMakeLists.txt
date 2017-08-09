cmake_minimum_required(VERSION 3.0)
project(i3-workspace-namer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++14 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -DDEBUG")

find_package(PkgConfig)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

set(Sources
    src/main.cpp
    src/ConfigParser.cpp
   )

set(Headers
    src/ConfigParser.h
   )

add_executable(i3-workspace-namer ${Sources} ${Headers})

add_subdirectory(i3ipc++)

include_directories(${I3IPCpp_INCLUDE_DIRS} ${JSONCPP_INCLUDE_DIRS})
link_directories(${I3IPCpp_LIBRARY_DIRS} ${JSONCPP_LIBRARY_DIRS})
target_link_libraries(i3-workspace-namer ${I3IPCpp_LIBRARIES} ${JSONCPP_LIBRARIES})
