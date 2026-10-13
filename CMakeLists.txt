#
# Copyright (C) 2025-2026 QuasarApp.
# Distributed under the GNU Lesser General Public License v3.0 (LGPL-3.0),
# see the accompanying LICENSE file.
#

cmake_minimum_required(VERSION 3.19)

project(OpenSimplex2 LANGUAGES C)
if(TARGET ${PROJECT_NAME})
  message("The ${PROJECT_NAME} already included in main Project")
  return()
endif()

include(submodule/CMake/QuasarApp.cmake)

set(CURRENT_PROJECT "OpenSimplex2Interface")

add_library(${CURRENT_PROJECT} INTERFACE)

set(SOURCE_C
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenSimplex2F.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenSimplex2F.h"
)

add_library(${CURRENT_PROJECT}C ${SOURCE_C})
target_include_directories(${CURRENT_PROJECT}C PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_options(${CURRENT_PROJECT}C PRIVATE -Wall -Wextra --pedantic)
# Link the C math library (libm, the "-lm" of the Makefile): the
# OpenSimplex2F.c implementation calls fabs() in its 4D scoring.
target_link_libraries(${CURRENT_PROJECT}C PRIVATE m)
target_link_libraries(${CURRENT_PROJECT} INTERFACE ${CURRENT_PROJECT}C)

option(OPENSIMPLEX2_TESTS "This option disables or enables tests of the ${PROJECT_NAME} project" ON)

initAll()

if (OPENSIMPLEX2_TESTS)
    add_subdirectory(tests)
else()
    message("The ${PROJECT_NAME} tests is disabled.")
endif()

