#pragma once
#include <nadir/core/math.hpp>

namespace nadir::geo {

struct Geodetic {
    double latitude_deg{};
    double longitude_deg{};
    double altitude_m{};
};

inline constexpr double a = 6378137.0;
inline constexpr double inverse_flattening = 298.257223563;
inline constexpr double flattening = 1.0 / inverse_flattening;
inline constexpr double e2 = flattening * (2.0 - flattening);
inline constexpr double gm = 3.986004418e14;
inline constexpr double omega = 7.292115e-5;

Vec3 geodetic_to_ecef(const Geodetic& g);
Geodetic ecef_to_geodetic(const Vec3& ecef);

}
