#pragma once

#include <nadir/units/length.hpp>
#include <nadir/units/speed.hpp>

namespace nadir::units {

inline constexpr MetersPerSecond speed_of_light{299'792'458.0};
inline constexpr Meters wgs84_semi_major_axis_m{6'378'137.0};
inline constexpr double wgs84_inverse_flattening = 298.257223563;

} // namespace nadir::units
