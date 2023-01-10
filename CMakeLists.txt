cmake_minimum_required(VERSION 3.23)
project(NJVM C)

set(CMAKE_C_STANDARD 11)

add_executable(NJVM njvm.c)

target_include_directories (NJVM PUBLIC include)

find_library(LIBRARY bigint lib)
target_link_libraries(NJVM LINK_PUBLIC ${LIBRARY})
