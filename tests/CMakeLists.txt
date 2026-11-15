# Copyright (C) 2026 QuasarApp Group DOO.
# Distributed under the GNU Lesser General Public License v3.0 (LGPL-3.0),
# see the accompanying LICENSE file.
#

get_filename_component(CURRENT_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(CURRENT_PROJECT "${PROJECT_NAME}${CURRENT_PROJECT_DIR}")

find_package(PNG REQUIRED)

# 2D/3D/4D OpenSimplex2F tests
set(SOURCE_2D_C
    "${CMAKE_CURRENT_SOURCE_DIR}/test_OpenSimplex2F.c"
)

add_executable(${CURRENT_PROJECT} ${SOURCE_2D_C})
target_include_directories(${CURRENT_PROJECT} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_compile_options(${CURRENT_PROJECT} PRIVATE -Wall -Wextra --pedantic)
target_link_libraries(${CURRENT_PROJECT} PRIVATE OpenSimplex2Interface m PNG::PNG)

# CTest framework: register 2D/3D/4D tests
set(TEST_NAMES_2D test_noise2 test_noise3 test_noise4)
foreach(test_name IN LISTS TEST_NAMES_2D)
    add_test(NAME ${test_name}
             COMMAND ${CURRENT_PROJECT} ${test_name}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

# 1D noise test - separate executable to avoid multiple main() definitions
set(SOURCE_1D_C
    "${CMAKE_CURRENT_SOURCE_DIR}/test_1d_simple.c"
)

add_executable(${PROJECT_NAME}_1d_simple ${SOURCE_1D_C})
target_include_directories(${PROJECT_NAME}_1d_simple PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_compile_options(${PROJECT_NAME}_1d_simple PRIVATE -Wall -Wextra --pedantic)
target_link_libraries(${PROJECT_NAME}_1d_simple PRIVATE OpenSimplex2Interface m)

# Register 1D test with CTest framework
add_test(NAME test_1d_simple
         COMMAND ${PROJECT_NAME}_1d_simple
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

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
