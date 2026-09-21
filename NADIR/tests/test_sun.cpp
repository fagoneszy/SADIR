#include <nadir/astro/sun.hpp>

#include <cmath>

int main() {
    const auto sun = nadir::astro::sun_position_teme_km(2451545.0);
    const double distance = sun.norm();
    if (!std::isfinite(distance) || std::abs(distance - 149597870.7) > 3'000'000.0) return 1;
    const auto itrf = nadir::astro::sun_position_itrf_km(2451545.0, {});
    if (!std::isfinite(itrf.x) || !std::isfinite(itrf.y) || !std::isfinite(itrf.z)) return 2;
    if (std::abs(itrf.norm() - distance) > 1.0) return 3;
    return nadir::astro::sun_position_teme_km(NAN).norm() == 0.0 ? 0 : 4;
}
