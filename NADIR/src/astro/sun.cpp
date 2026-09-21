#include <nadir/astro/sun.hpp>

#include <nadir/geo/frames.hpp>

#include <cmath>

namespace nadir::astro {
namespace {
constexpr double au_km = 149597870.7;
constexpr double deg_to_rad = 0.017453292519943295769;
double wrap_degrees(double value) noexcept {
    value = std::fmod(value, 360.0);
    return value < 0.0 ? value + 360.0 : value;
}
}

orbit::Vec3d sun_position_teme_km(double jd_utc) noexcept {
    if (!std::isfinite(jd_utc)) return {};
    const double n = jd_utc - 2451545.0;
    const double mean_longitude = wrap_degrees(280.460 + 0.9856474 * n) * deg_to_rad;
    const double mean_anomaly = wrap_degrees(357.528 + 0.9856003 * n) * deg_to_rad;
    const double ecliptic_longitude = mean_longitude + (1.915 * std::sin(mean_anomaly) +
        0.020 * std::sin(2.0 * mean_anomaly)) * deg_to_rad;
    const double distance_au = 1.00014 - 0.01671 * std::cos(mean_anomaly) -
        0.00014 * std::cos(2.0 * mean_anomaly);
    const double obliquity = (23.439 - 0.0000004 * n) * deg_to_rad;
    const double radius = distance_au * au_km;
    return {radius * std::cos(ecliptic_longitude),
            radius * std::cos(obliquity) * std::sin(ecliptic_longitude),
            radius * std::sin(obliquity) * std::sin(ecliptic_longitude)};
}

orbit::Vec3d sun_position_itrf_km(double jd_utc, const geo::EopRecord& eop) noexcept {
    const auto sun = sun_position_teme_km(jd_utc);
    const geo::StateVector teme{{sun.x * 1000.0, sun.y * 1000.0, sun.z * 1000.0}, {}};
    const auto itrf = geo::teme_to_itrf(teme, jd_utc, eop);
    return {itrf.position.x / 1000.0, itrf.position.y / 1000.0, itrf.position.z / 1000.0};
}

} // namespace nadir::astro
