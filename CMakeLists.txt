cmake_minimum_required(VERSION 2.8)
project(multi_cpp_final)

set(CMAKE_CXX_STANDARD 11)

SET(HTTP_PARSER_PATH libs/httpparser/src)

include_directories(${HTTP_PARSER_PATH})

add_executable(final main.cpp)
target_link_libraries(final pthread)

