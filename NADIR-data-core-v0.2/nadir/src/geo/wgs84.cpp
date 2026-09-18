#include <nadir/geo/wgs84.hpp>
#include <cmath>

namespace nadir::geo {

Vec3 geodetic_to_ecef(const Geodetic& g) {
    const double lat = g.latitude_deg * deg_to_rad;
    const double lon = g.longitude_deg * deg_to_rad;
    const double sl = std::sin(lat);
    const double cl = std::cos(lat);
    const double so = std::sin(lon);
    const double co = std::cos(lon);
    const double n = a / std::sqrt(1.0 - e2 * sl * sl);
    return {(n + g.altitude_m) * cl * co, (n + g.altitude_m) * cl * so, (n * (1.0 - e2) + g.altitude_m) * sl};
}

Geodetic ecef_to_geodetic(const Vec3& p) {
    const double lon = std::atan2(p.y, p.x);
    const double r = std::hypot(p.x, p.y);
    double lat = std::atan2(p.z, r * (1.0 - e2));
    double alt = 0.0;
    for (int i = 0; i < 8; ++i) {
        const double s = std::sin(lat);
        const double n = a / std::sqrt(1.0 - e2 * s * s);
        alt = r / std::cos(lat) - n;
        lat = std::atan2(p.z, r * (1.0 - e2 * n / (n + alt)));
    }
    return {lat * rad_to_deg, lon * rad_to_deg, alt};
}

}
