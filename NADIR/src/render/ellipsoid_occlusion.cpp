#include <nadir/render/ellipsoid_occlusion.hpp>

#include <algorithm>
#include <cmath>

namespace nadir::render {

namespace {

constexpr double epsilon = 1.0e-9;

bool finite(const math::Vec3d& value) noexcept {
    return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
}

} // namespace

Ellipsoid wgs84_ellipsoid() noexcept {
    constexpr double semi_major = 6378137.0;
    constexpr double flattening = 1.0 / 298.257223563;
    return {semi_major, semi_major * (1.0 - flattening)};
}

bool segment_intersects_ellipsoid(const math::Vec3d& origin, const math::Vec3d& target,
                                  const Ellipsoid& ellipsoid) noexcept {
    if (!finite(origin) || !finite(target) || !(ellipsoid.semi_major_m > 0.0) ||
        !(ellipsoid.semi_minor_m > 0.0) || !std::isfinite(ellipsoid.semi_major_m) ||
        !std::isfinite(ellipsoid.semi_minor_m)) return false;

    const auto direction = target - origin;
    const math::Vec3d scaled_origin{origin.x / ellipsoid.semi_major_m,
                                    origin.y / ellipsoid.semi_major_m,
                                    origin.z / ellipsoid.semi_minor_m};
    const math::Vec3d scaled_direction{direction.x / ellipsoid.semi_major_m,
                                       direction.y / ellipsoid.semi_major_m,
                                       direction.z / ellipsoid.semi_minor_m};
    const double a = scaled_direction.dot(scaled_direction);
    if (!std::isfinite(a) || a <= epsilon) return false;
    const double b = 2.0 * scaled_origin.dot(scaled_direction);
    const double c = scaled_origin.dot(scaled_origin) - 1.0;
    double discriminant = b * b - 4.0 * a * c;
    if (!std::isfinite(discriminant) || discriminant < -epsilon) return false;
    discriminant = std::max(0.0, discriminant);
    const double root = std::sqrt(discriminant);
    const double t0 = (-b - root) / (2.0 * a);
    const double t1 = (-b + root) / (2.0 * a);
    return (t0 > epsilon && t0 < 1.0 - epsilon) || (t1 > epsilon && t1 < 1.0 - epsilon);
}

bool occluded_by_ellipsoid(const math::Vec3d& camera, const math::Vec3d& object,
                           const Ellipsoid& ellipsoid) noexcept {
    return segment_intersects_ellipsoid(camera, object, ellipsoid);
}

} // namespace nadir::render
