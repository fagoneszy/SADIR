#include <nadir/render/projection.hpp>
#include <cmath>
#include <limits>

namespace nadir::render {

namespace {
constexpr double pi = 3.141592653589793238462643383279502884;
}

ProjectedPoint project_perspective(
    const nadir::math::Vec3d& view,
    double fov_deg,
    double aspect,
    double near_plane,
    double far_plane
) {
    ProjectedPoint out{};

    const double depth = -view.z;

    if (!std::isfinite(depth) || depth <= near_plane || depth >= far_plane) {
        return ProjectedPoint{};
    }

    const double fov_rad = fov_deg * pi / 180.0;
    const double scale = 1.0 / std::tan(fov_rad * 0.5);

    out.x_ndc = (view.x * scale) / (depth * aspect);
    out.y_ndc = (view.y * scale) / depth;
    out.depth = depth;

    out.visible = std::abs(out.x_ndc) <= 1.0 && std::abs(out.y_ndc) <= 1.0;

    return out;
}

ScreenPoint ndc_to_viewport(
    const ProjectedPoint& p,
    int width,
    int height
) {
    if (!p.visible) {
        return ScreenPoint{};
    }

    const double u = p.x_ndc * 0.5 + 0.5;
    const double v = p.y_ndc * 0.5 + 0.5;

    return ScreenPoint{
        u * static_cast<double>(width - 1),
        (1.0 - v) * static_cast<double>(height - 1),
        p.depth,
        true
    };
}

}