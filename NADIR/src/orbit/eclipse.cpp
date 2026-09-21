#include <nadir/orbit/eclipse.hpp>

#include <algorithm>
#include <cmath>

namespace nadir::orbit {
Illumination classify_earth_eclipse(const Vec3d& satellite, const Vec3d& sun,
                                    double earth_radius, double sun_radius) noexcept {
    const auto finite = [](const Vec3d& v) { return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z); };
    if (!finite(satellite) || !finite(sun) || earth_radius <= 0.0 || sun_radius <= 0.0) return Illumination::Invalid;
    const auto sat_to_sun = sun - satellite;
    const double sun_distance = sat_to_sun.norm();
    const double satellite_distance = satellite.norm();
    if (sun_distance <= sun_radius || satellite_distance <= earth_radius) return Illumination::Invalid;
    const auto earth_direction = (satellite * -1.0).normalized();
    const auto sun_direction = sat_to_sun.normalized();
    const double separation = std::acos(std::clamp(earth_direction.dot(sun_direction), -1.0, 1.0));
    const double earth_angular = std::asin(std::clamp(earth_radius / satellite_distance, 0.0, 1.0));
    const double sun_angular = std::asin(std::clamp(sun_radius / sun_distance, 0.0, 1.0));
    if (separation >= earth_angular + sun_angular) return Illumination::Sunlit;
    if (earth_angular >= separation + sun_angular) return Illumination::Umbra;
    return Illumination::Penumbra;
}
} // namespace nadir::orbit
