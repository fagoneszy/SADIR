cmake_minimum_required(VERSION 3.24)
project(nadir VERSION 0.2.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_library(nadir_core
    src/core/terminal.cpp
    src/core/time.cpp
    src/core/sha256.cpp
    src/data/source.cpp
    src/data/http.cpp
    src/data/cache.cpp
    src/data/sync.cpp
    src/astro/bodies.cpp
    src/astro/horizons.cpp
    src/geo/wgs84.cpp
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
include(CTest)
if(BUILD_TESTING)
    add_executable(nadir_tests tests/test_main.cpp)
    target_link_libraries(nadir_tests PRIVATE nadir_core)
    add_test(NAME nadir_tests COMMAND nadir_tests)
endif()
