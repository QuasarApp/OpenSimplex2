#
# Copyright (C) 2026 QuasarApp Group DOO.
# Distributed under the GNU Lesser General Public License v3.0 (LGPL-3.0),
# see the accompanying LICENSE file.
#

get_filename_component(CURRENT_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(CURRENT_PROJECT "${PROJECT_NAME}${CURRENT_PROJECT_DIR}")

find_package(PNG REQUIRED)

set(SOURCE_C
    "${CMAKE_CURRENT_SOURCE_DIR}/test_OpenSimplex2F.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_1d_simple.c"
)

add_executable(${CURRENT_PROJECT} ${SOURCE_C})
target_include_directories(${CURRENT_PROJECT} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_compile_options(${CURRENT_PROJECT} PRIVATE -Wall -Wextra --pedantic)
target_link_libraries(${CURRENT_PROJECT} PRIVATE OpenSimplex2Interface m PNG::PNG)

# Add 1D simple test (doesn't need PNG)
add_executable(test_1d_simple "${CMAKE_CURRENT_SOURCE_DIR}/test_1d_simple.c")
target_include_directories(test_1d_simple PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_compile_options(test_1d_simple PRIVATE -Wall -Wextra --pedantic)
target_link_libraries(test_1d_simple PRIVATE OpenSimplex2Interface m)

initTests()
addTests(${PROJECT_NAME} ${CURRENT_PROJECT})
add_test(${PROJECT_NAME} test_1d_simple)
