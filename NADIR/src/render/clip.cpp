#include <nadir/render/clip.hpp>

#include <algorithm>
#include <cmath>

namespace nadir::render {

ClippedSegment3D clip_depth(const math::Vec3d& a, const math::Vec3d& b,
                             double near_plane, double far_plane) noexcept {
    const double depth_a = -a.z;
    const double depth_b = -b.z;
    if (!(near_plane > 0.0 && far_plane >= near_plane) || !std::isfinite(depth_a) ||
        !std::isfinite(depth_b) || std::max(depth_a, depth_b) < near_plane ||
        std::min(depth_a, depth_b) > far_plane) return {};

    auto first = a;
    auto second = b;
    const auto clip_plane = [&](double plane, bool lower) {
        const double first_depth = -first.z;
        const double second_depth = -second.z;
        const bool first_inside = lower ? first_depth >= plane : first_depth <= plane;
        const bool second_inside = lower ? second_depth >= plane : second_depth <= plane;
        if (first_inside && second_inside) return true;
        if (!first_inside && !second_inside) return false;
        const auto hit = first + (second - first) *
            ((plane - first_depth) / (second_depth - first_depth));
        if (!first_inside) first = hit;
        else second = hit;
        return true;
    };
    return clip_plane(near_plane, true) && clip_plane(far_plane, false)
        ? ClippedSegment3D{first, second, true} : ClippedSegment3D{};
}

ClippedNdcSegment clip_ndc(NdcPoint a, NdcPoint b) noexcept {
    if (!a.visible || !b.visible || !std::isfinite(a.x) || !std::isfinite(a.y) ||
        !std::isfinite(a.inverse_depth) || !std::isfinite(b.x) || !std::isfinite(b.y) ||
        !std::isfinite(b.inverse_depth)) return {};

    double enter = 0.0;
    double exit = 1.0;
    const auto clip_axis = [&](double first, double second) {
        const double delta = second - first;
        if (std::abs(delta) < 1e-12) return first >= -1.0 && first <= 1.0;
        double low = (-1.0 - first) / delta;
        double high = (1.0 - first) / delta;
        if (low > high) std::swap(low, high);
        enter = std::max(enter, low);
        exit = std::min(exit, high);
        return enter <= exit;
    };
    if (!clip_axis(a.x, b.x) || !clip_axis(a.y, b.y)) return {};

    const auto interpolate = [](double first, double second, double t) {
        return first + (second - first) * t;
    };
    const auto point_at = [&](double t) {
        return NdcPoint{interpolate(a.x, b.x, t), interpolate(a.y, b.y, t),
                        interpolate(a.inverse_depth, b.inverse_depth, t), true};
    };
    return {point_at(enter), point_at(exit), true, enter > 1e-12 || exit < 1.0 - 1e-12};
}

} // namespace nadir::render
