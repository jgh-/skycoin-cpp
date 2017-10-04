cmake_minimum_required(VERSION 3.5)
project(skycoin)
set(CMAKE_BUILD_TYPE Debug)

include_directories(include cpp_modules/include)
link_directories(cpp_modules/lib)

file(GLOB SOURCES "src/*.cc" "src/net/tcp/*.cc" "src/net/udp/*.cc" "src/coin/*.cc")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wpedantic -Werror -pthread")

add_library(skycoin STATIC ${SOURCES})
add_executable(node "src/cmd/node.cc")
target_link_libraries(node skycoin)

add_executable(test_block "test/test_block.cc")
target_link_libraries(test_block skycoin)

add_executable(test_peer "test/test_peer.cc")
target_link_libraries(test_peer skycoin)

add_executable(test_tx "test/test_tx.cc")
target_link_libraries(test_tx skycoin)