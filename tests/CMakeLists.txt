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
)

add_executable(${CURRENT_PROJECT} ${SOURCE_C})
target_include_directories(${CURRENT_PROJECT} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_compile_options(${CURRENT_PROJECT} PRIVATE -Wall -Wextra --pedantic)
target_link_libraries(${CURRENT_PROJECT} PRIVATE OpenSimplex2Interface m PNG::PNG)

# CTest framework: every test function of the main test executable
# (test_noise2, test_noise3, test_noise4) is registered as a separate
# CTest test, so each of them can be run individually, e.g.:
#     ctest -R test_noise2
# or all together (the `test` build target / `ctest` from the build dir).
# The generated PNG images are written to the build directory, not to the
# source tree.
set(TEST_NAMES test_noise2 test_noise3 test_noise4)

foreach(test_name IN LISTS TEST_NAMES)
    add_test(NAME ${test_name}
             COMMAND ${CURRENT_PROJECT} ${test_name}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
