cmake_minimum_required(VERSION 3.19)

set(CURRENT_PROJECT "OpenSimplex2Interface")

option(OPENSIMPLEX_C "Enable/Disable build of the old C implementation" OFF)
option(OPENSIMPLEX_RUST "Enable/Disable build of the main Rust implementation" ON)


add_library(${CURRENT_PROJECT} INTERFACE)

if (OPENSIMPLEX_RUST)

    include(FetchContent)

    FetchContent_Declare(
        Corrosion
        GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
        GIT_TAG v0.5.2

    )
    FetchContent_MakeAvailable(Corrosion)
    corrosion_import_crate(MANIFEST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Cargo.toml ALL_FEATURES)

    target_link_libraries(${CURRENT_PROJECT} INTERFACE "opensimplex2")
    target_include_directories(${CURRENT_PROJECT} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/rust/")
    target_sources(${CURRENT_PROJECT} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/rust/OpenSimplex2.h")

endif()

if (OPENSIMPLEX_C)

    set(SOURCE_CPP
        "${CMAKE_CURRENT_SOURCE_DIR}/_old/c/OpenSimplex2F.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/_old/c/OpenSimplex2F.h"
    )

    add_library(${CURRENT_PROJECT}C ${SOURCE_CPP})
    target_include_directories(${CURRENT_PROJECT}C PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/_old/c/")
    target_compile_options(${CURRENT_PROJECT}C PRIVATE -Wall -Wextra --pedantic)
    target_link_libraries(${CURRENT_PROJECT} INTERFACE ${CURRENT_PROJECT}C)

endif()
