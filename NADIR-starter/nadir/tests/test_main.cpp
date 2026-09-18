#include <nadir/geo/wgs84.hpp>
#include <nadir/satellite/tle.hpp>
#include <cmath>
#include <iostream>

int main() {
    const nadir::geo::Geodetic g{0.0, 0.0, 0.0};
    const auto e = nadir::geo::geodetic_to_ecef(g);
    if (std::abs(e.x - nadir::geo::a) > 0.001) return 1;
    if (std::abs(e.y) > 0.001 || std::abs(e.z) > 0.001) return 2;
    const auto back = nadir::geo::ecef_to_geodetic(e);
    if (std::abs(back.latitude_deg) > 1e-8 || std::abs(back.longitude_deg) > 1e-8) return 3;
    std::cout << "OK\n";
    return 0;
}
