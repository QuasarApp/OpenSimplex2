cmake_minimum_required(VERSION 3.19)

set(CURRENT_PROJECT "OpenSimplex2Interface")

add_library(${CURRENT_PROJECT} INTERFACE)

set(SOURCE_C
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenSimplex2F.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenSimplex2F.h"
)

add_library(${CURRENT_PROJECT}C ${SOURCE_C})
target_include_directories(${CURRENT_PROJECT}C PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_options(${CURRENT_PROJECT}C PRIVATE -Wall -Wextra --pedantic)
target_link_libraries(${CURRENT_PROJECT} INTERFACE ${CURRENT_PROJECT}C)
