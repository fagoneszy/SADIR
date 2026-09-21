#pragma once

#include <nadir/orbit/vector.hpp>

namespace nadir::orbit {

enum class Illumination { Sunlit, Penumbra, Umbra, Invalid };

Illumination classify_earth_eclipse(const Vec3d& satellite_km, const Vec3d& sun_km,
                                    double earth_radius_km = 6378.137,
                                    double sun_radius_km = 695700.0) noexcept;

} // namespace nadir::orbit
