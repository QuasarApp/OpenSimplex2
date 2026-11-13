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

# Cross-platform noise benchmark (issue #31): evaluates every OpenSimplex2F
# noise function with the same seed and the same input parameters on every
# platform and prints the exact binary64 bit pattern of every result. The
# .github/workflows/cross-platform-compare.yml workflow runs it on Linux,
# Windows and macOS and compares the produced report byte-for-byte.
set(CROSS_PLATFORM_BENCHMARK "${PROJECT_NAME}_cross_platform")

add_executable(${CROSS_PLATFORM_BENCHMARK}
    "${CMAKE_CURRENT_SOURCE_DIR}/test_OpenSimplex2F_cross_platform.c"
)
target_include_directories(${CROSS_PLATFORM_BENCHMARK} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_compile_options(${CROSS_PLATFORM_BENCHMARK} PRIVATE -Wall -Wextra --pedantic)
target_link_libraries(${CROSS_PLATFORM_BENCHMARK} PRIVATE OpenSimplex2Interface m)

# The benchmark report must be bit-identical on all platforms, but arm64
# compilers (Apple Silicon) may contract a*b+c into FMA instructions, which
# changes the least significant bits of the results. Disable FMA contraction
# for the GNU/Clang builds (on x86-64 this is a no-op). -ffp-contract=off is
# supported by both GCC and Clang (-mno-fma is rejected by Apple Clang on
# arm64 macOS).
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${CROSS_PLATFORM_BENCHMARK} PRIVATE -ffp-contract=off)
endif()

# The cross-platform comparison workflow
# (.github/workflows/cross-platform-compare.yml) builds and runs this target
# on Linux, Windows and macOS. It is intentionally not registered as a CTest
# test, so it is not part of the `test` build target (the benchmark needs no
# cqtdeployer deployment). The benchmark writes
# noise_benchmark_results.txt into the build/cross_platform working
# directory (the executable creates it).
add_custom_target(${PROJECT_NAME}_cross_platform_benchmark
    COMMAND $<TARGET_FILE:${CROSS_PLATFORM_BENCHMARK}> "${CMAKE_BINARY_DIR}/cross_platform/noise_benchmark_results.txt"
    DEPENDS ${CROSS_PLATFORM_BENCHMARK}
    COMMENT "Running the OpenSimplex2F cross-platform noise benchmark"
    VERBATIM)
