cmake_minimum_required(VERSION 3.24)
project(nadir VERSION 0.2.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# OPTIONAL MODULES
# ============================================================================
option(BUILD_TIME_MODULE "Build NADIR Time Core module" OFF)
option(BUILD_TESTING "Build and run tests" OFF)

# ============================================================================
# CORE LIBRARY (existing foundation + v0.2 DATA CORE enhancements)
# ============================================================================
add_library(nadir_core
    src/core/terminal.cpp
    src/core/time.cpp
    src/core/sha256.cpp
    src/geo/wgs84.cpp
    src/data/source.cpp
    src/data/http.cpp
    src/data/cache.cpp
    src/data/sync.cpp
    src/astro/bodies.cpp
    src/astro/horizons.cpp
    src/render/framebuffer.cpp
    src/render/line.cpp
    src/render/earth.cpp
    src/satellite/tle.cpp
    src/spacecraft/architecture.cpp
    src/model/obj.cpp
    src/format/ndr.cpp
    src/tui/app.cpp
)
target_include_directories(nadir_core PUBLIC include)
if(MSVC)
    target_compile_options(nadir_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(nadir_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(nadir src/main.cpp)
target_link_libraries(nadir PRIVATE nadir_core)

# ============================================================================
# TIME MODULE (Nadir Clock - camada DATA CORE base)
# ============================================================================
if(BUILD_TIME_MODULE)
    add_library(nadir_time
        src/core/time/clock.cpp
    )
    target_include_directories(nadir_time PUBLIC include/nadir/time)
    target_link_libraries(nadir_time PUBLIC nadir_core)

    # Expose time symbols for use by other modules
    add_definitions(-DHAVE_NADIR_TIME)
endif()

# ============================================================================
# TUI
# ============================================================================
add_executable(nadir src/main.cpp)
target_link_libraries(nadir PRIVATE nadir_core)
if(BUILD_TIME_MODULE)
    target_link_libraries(nadir PRIVATE nadir_time)
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(nadir_tests tests/test_main.cpp tests/test_clock.cpp)
    target_link_libraries(nadir_tests PRIVATE nadir_core)
    if(BUILD_TIME_MODULE)
        target_link_libraries(nadir_tests PRIVATE nadir_time)
    endif()
    add_test(NAME nadir_tests COMMAND nadir_tests)
endif()