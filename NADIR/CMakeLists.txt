cmake_minimum_required(VERSION 3.24)
project(nadir VERSION 0.3.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_library(nadir_math INTERFACE)
target_include_directories(nadir_math INTERFACE include)
add_library(nadir_units INTERFACE)
target_include_directories(nadir_units INTERFACE include)
target_link_libraries(nadir_units INTERFACE nadir_math)
add_library(nadir_time INTERFACE)
target_include_directories(nadir_time INTERFACE include)
add_library(nadir_frames INTERFACE)
target_include_directories(nadir_frames INTERFACE include)
target_link_libraries(nadir_frames INTERFACE nadir_time)
add_library(nadir_state INTERFACE)
target_include_directories(nadir_state INTERFACE include)
target_link_libraries(nadir_state INTERFACE nadir_math nadir_units nadir_time nadir_frames)
add_library(nadir_scene
    src/render/scene.cpp
    src/render/scene_builder.cpp
)
target_include_directories(nadir_scene PUBLIC include)
target_link_libraries(nadir_scene PUBLIC nadir_state nadir_math)

add_library(nadir_core
    src/core/terminal.cpp
    src/core/time.cpp
    src/core/sha256.cpp
    src/core/json.cpp
    src/core/time_controller.cpp
    src/time/instant.cpp
    src/time/utc.cpp
    src/time/leap_seconds.cpp
    src/time/julian_date.cpp
    src/time/time_context.cpp
    src/data/source.cpp
    src/data/preset.cpp
    src/data/http.cpp
    src/data/cache.cpp
    src/data/sync.cpp
    src/astro/bodies.cpp
    src/astro/targets.cpp
    src/astro/horizons.cpp
    src/astro/omm.cpp
    src/astro/catalog.cpp
    src/astro/sun.cpp
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
    src/render/picking.cpp
    src/render/depth_buffer.cpp
    src/render/display_transform.cpp
    src/render/clip.cpp
    src/render/ellipsoid_occlusion.cpp
    src/render/renderer3d.cpp
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
    src/orbit/batch.cpp
    src/orbit/snapshot_buffer.cpp
    src/orbit/sgp4_init.cpp
    src/orbit/sgp4_deep.cpp
    src/orbit/pass_predictor.cpp
    src/orbit/eclipse.cpp
    src/orbit/ground_track.cpp
    src/orbit/scene_adapter.cpp
    src/orbit/tracker.cpp
    third_party/vallado/SGP4.cpp
    src/math/quat.cpp
    src/space/body.cpp
    src/space/solar_catalog.cpp
    src/space/ephemeris_provider.cpp
)
target_include_directories(nadir_core PUBLIC include)
target_link_libraries(nadir_core PRIVATE nadir_scene)
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
add_executable(nadir_render_demo tests/render_demo3d.cpp)
target_link_libraries(nadir_render_demo PRIVATE nadir_core)
include(CTest)
if(BUILD_TESTING)
    add_executable(nadir_tests tests/test_main.cpp)
    target_link_libraries(nadir_tests PRIVATE nadir_core)
    add_test(NAME nadir_tests COMMAND nadir_tests)

    add_executable(nadir_render_tests tests/test_render.cpp)
    target_link_libraries(nadir_render_tests PRIVATE nadir_core)
    add_test(NAME nadir_render_tests COMMAND nadir_render_tests)

    add_executable(nadir_units_tests tests/test_units.cpp)
    target_link_libraries(nadir_units_tests PRIVATE nadir_units)
    add_test(NAME nadir_units_tests COMMAND nadir_units_tests)

    add_executable(nadir_time_tests tests/test_time.cpp)
    target_link_libraries(nadir_time_tests PRIVATE nadir_core)
    add_test(NAME nadir_time_tests COMMAND nadir_time_tests)

    add_executable(nadir_frames_tests tests/test_frames.cpp)
    target_link_libraries(nadir_frames_tests PRIVATE nadir_core)
    add_test(NAME nadir_frames_tests COMMAND nadir_frames_tests)

    add_executable(nadir_state_tests tests/test_state.cpp)
    target_link_libraries(nadir_state_tests PRIVATE nadir_state)
    add_test(NAME nadir_state_tests COMMAND nadir_state_tests)

    add_executable(nadir_scene_tests tests/test_scene.cpp)
    target_link_libraries(nadir_scene_tests PRIVATE nadir_scene)
    add_test(NAME nadir_scene_tests COMMAND nadir_scene_tests)

    add_executable(nadir_renderer3d_tests tests/test_renderer3d.cpp)
    target_link_libraries(nadir_renderer3d_tests PRIVATE nadir_core)
    add_test(NAME nadir_renderer3d_tests COMMAND nadir_renderer3d_tests)

    add_executable(nadir_ellipsoid_occlusion_tests tests/test_ellipsoid_occlusion.cpp)
    target_link_libraries(nadir_ellipsoid_occlusion_tests PRIVATE nadir_core)
    add_test(NAME nadir_ellipsoid_occlusion_tests COMMAND nadir_ellipsoid_occlusion_tests)

    add_executable(nadir_earth_mesh_tests tests/test_earth_mesh.cpp)
    target_link_libraries(nadir_earth_mesh_tests PRIVATE nadir_core)
    add_test(NAME nadir_earth_mesh_tests COMMAND nadir_earth_mesh_tests)

    add_executable(nadir_omm_tests tests/test_omm.cpp)
    target_link_libraries(nadir_omm_tests PRIVATE nadir_core)
    add_test(NAME nadir_omm_tests COMMAND nadir_omm_tests)

    add_executable(nadir_pass_predictor_tests tests/test_pass_predictor.cpp)
    target_link_libraries(nadir_pass_predictor_tests PRIVATE nadir_core)
    add_test(NAME nadir_pass_predictor_tests COMMAND nadir_pass_predictor_tests)

    add_executable(nadir_eclipse_tests tests/test_eclipse.cpp)
    target_link_libraries(nadir_eclipse_tests PRIVATE nadir_core)
    add_test(NAME nadir_eclipse_tests COMMAND nadir_eclipse_tests)

    add_executable(nadir_tracker_tests tests/test_tracker.cpp)
    target_link_libraries(nadir_tracker_tests PRIVATE nadir_core)
    add_test(NAME nadir_tracker_tests COMMAND nadir_tracker_tests)

    add_executable(nadir_ground_track_tests tests/test_ground_track.cpp)
    target_link_libraries(nadir_ground_track_tests PRIVATE nadir_core)
    add_test(NAME nadir_ground_track_tests COMMAND nadir_ground_track_tests)

    add_executable(nadir_sun_tests tests/test_sun.cpp)
    target_link_libraries(nadir_sun_tests PRIVATE nadir_core)
    add_test(NAME nadir_sun_tests COMMAND nadir_sun_tests)

    add_executable(nadir_eop_tests tests/test_eop.cpp)
    target_link_libraries(nadir_eop_tests PRIVATE nadir_core)
    add_test(NAME nadir_eop_tests COMMAND nadir_eop_tests)

    add_executable(nadir_scene_adapter_tests tests/test_scene_adapter.cpp)
    target_link_libraries(nadir_scene_adapter_tests PRIVATE nadir_core)
    add_test(NAME nadir_scene_adapter_tests COMMAND nadir_scene_adapter_tests)

    add_executable(nadir_catalog_tests tests/test_catalog.cpp)
    target_link_libraries(nadir_catalog_tests PRIVATE nadir_core)
    add_test(NAME nadir_catalog_tests COMMAND nadir_catalog_tests)

    add_executable(nadir_picking_tests tests/test_picking.cpp)
    target_link_libraries(nadir_picking_tests PRIVATE nadir_core)
    add_test(NAME nadir_picking_tests COMMAND nadir_picking_tests)

    add_executable(nadir_batch_tests tests/test_batch.cpp)
    target_link_libraries(nadir_batch_tests PRIVATE nadir_core)
    add_test(NAME nadir_batch_tests COMMAND nadir_batch_tests)

    add_executable(nadir_snapshot_buffer_tests tests/test_snapshot_buffer.cpp)
    target_link_libraries(nadir_snapshot_buffer_tests PRIVATE nadir_core)
    add_test(NAME nadir_snapshot_buffer_tests COMMAND nadir_snapshot_buffer_tests)

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
