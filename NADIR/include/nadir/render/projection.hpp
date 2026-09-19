#pragma once

#include <nadir/math/vec3.hpp>

namespace nadir::render {

struct ProjectedPoint {
    double x_ndc{};
    double y_ndc{};
    double depth{};
    bool visible{false};
};

struct ScreenPoint {
    double x{};
    double y{};
    double depth{};
    bool visible{false};
};

ProjectedPoint project_perspective(
    const nadir::math::Vec3d& view,
    double fov_deg,
    double aspect,
    double near_plane,
    double far_plane
);

ScreenPoint ndc_to_viewport(
    const ProjectedPoint& p,
    int width,
    int height
);

}