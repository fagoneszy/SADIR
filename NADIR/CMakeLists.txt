cmake_minimum_required(VERSION 3.24)
project(nadir VERSION 0.3.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_library(nadir_core
    src/core/terminal.cpp
    src/core/time.cpp
    src/core/sha256.cpp
    src/core/json.cpp
    src/core/time_controller.cpp
    src/data/source.cpp
    src/data/preset.cpp
    src/data/http.cpp
    src/data/cache.cpp
    src/data/sync.cpp
    src/astro/bodies.cpp
    src/astro/targets.cpp
    src/astro/horizons.cpp
    src/astro/omm.cpp
    src/geo/wgs84.cpp
    src/geo/eop.cpp
    src/geo/frames.cpp
    src/earth/seismic.cpp
    src/earth/space_weather.cpp
    src/earth/fireball.cpp
    src/render/framebuffer.cpp
    src/render/line.cpp
    src/render/earth.cpp
    src/render/frame_clock.cpp
    src/render/camera.cpp
    src/render/projection.cpp
    src/render/scene.cpp
    src/render/phosphor_buffer.cpp
    src/render/presenter.cpp
    src/render/earth_mesh.cpp
    src/satellite/tle.cpp
    src/spacecraft/architecture.cpp
    src/model/obj.cpp
    src/format/ndr.cpp
    src/system/station.cpp
    src/tui/app.cpp
    src/orbit/sgp4.cpp
    src/orbit/sgp4_init.cpp
    src/orbit/sgp4_deep.cpp
    src/math/quat.cpp
    src/space/body.cpp
    src/space/solar_catalog.cpp
    src/space/ephemeris_provider.cpp
)
target_include_directories(nadir_core PUBLIC include)
if(MSVC)
    target_compile_options(nadir_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(nadir_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(WIN32)
    target_link_libraries(nadir_core PRIVATE iphlpapi ws2_32)
endif()
add_executable(nadir src/main.cpp)
target_link_libraries(nadir PRIVATE nadir_core)
include(CTest)
if(BUILD_TESTING)
    add_executable(nadir_tests tests/test_main.cpp)
    target_link_libraries(nadir_tests PRIVATE nadir_core)
    add_test(NAME nadir_tests COMMAND nadir_tests)

    add_executable(nadir_render_tests tests/test_render.cpp)
    target_link_libraries(nadir_render_tests PRIVATE nadir_core)
    add_test(NAME nadir_render_tests COMMAND nadir_render_tests)

    add_executable(nadir_sgp4_verification tests/test_sgp4_verification.cpp)
    target_link_libraries(nadir_sgp4_verification PRIVATE nadir_core)
    add_test(NAME nadir_sgp4_verification COMMAND nadir_sgp4_verification)
    set_tests_properties(nadir_sgp4_verification PROPERTIES LABELS science)

    add_custom_target(science-gate
        COMMAND ${CMAKE_CTEST_COMMAND} -R nadir_sgp4_verification --output-on-failure
        DEPENDS nadir_sgp4_verification
        COMMENT "Running science gate (SGP4 Vallado verification)"
    )
endif()
