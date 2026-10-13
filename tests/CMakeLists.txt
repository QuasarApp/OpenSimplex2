#
# Copyright (C) 2025-2026 QuasarApp.
# Distributed under the GNU Lesser General Public License v3.0 (LGPL-3.0),
# see the accompanying LICENSE file.
#

get_filename_component(CURRENT_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(CURRENT_PROJECT "${PROJECT_NAME}${CURRENT_PROJECT_DIR}")

find_package(PNG REQUIRED)

set(SOURCE_C
    "${CMAKE_CURRENT_SOURCE_DIR}/test_OpenSimplex2F.c"
)

add_executable(${CURRENT_PROJECT} ${SOURCE_C})
target_include_directories(${CURRENT_PROJECT} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_compile_options(${CURRENT_PROJECT} PRIVATE -Wall -Wextra --pedantic)
target_link_libraries(${CURRENT_PROJECT} PRIVATE OpenSimplex2Interface m PNG::PNG)

initTests()
addTests(${PROJECT_NAME} ${CURRENT_PROJECT})
