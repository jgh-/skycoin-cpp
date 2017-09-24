cmake_minimum_required(VERSION 3.5)
project(skywire)
set(CMAKE_BUILD_TYPE Debug)

include_directories(include cpp_modules/include)
link_directories(cpp_modules/lib)

file(GLOB SOURCES "src/*.cc" "src/net/tcp/*.cc" "src/net/udp/*.cc")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wpedantic -Werror -pthread")

add_library(skywire STATIC ${SOURCES})
add_executable(node "src/cmd/node.cc")
target_link_libraries(node skywire)