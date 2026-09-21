#pragma once
#include <cstdint>
namespace nadir::render { struct RenderStats { std::uint64_t points_submitted{},points_visible{},segments_submitted{},segments_clipped{},segments_visible{},depth_tests{},depth_passes{}; double render_ms{}; }; }
