#pragma once
#include <nadir/math/vec3.hpp>
namespace nadir::render { enum class DisplayScale { Linear, FocusRelative }; struct DisplayTransform { math::Vec3d physical_origin_m{}; double meters_per_render_unit{6378137.0}; DisplayScale scale{DisplayScale::Linear}; }; math::Vec3d to_render_space(const math::Vec3d&,const DisplayTransform&) noexcept; }
