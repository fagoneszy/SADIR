#include <nadir/units/angle.hpp>
#include <nadir/units/constants.hpp>
#include <nadir/units/duration.hpp>
#include <nadir/units/frequency.hpp>
#include <nadir/units/length.hpp>
#include <nadir/units/speed.hpp>
#include <nadir/units/vector_units.hpp>

#include <cmath>
#include <numbers>
#include <type_traits>

static_assert(!std::is_convertible_v<nadir::units::Meters, double>);
static_assert(!std::is_convertible_v<nadir::units::Degrees, nadir::units::Radians>);
static_assert(!std::is_convertible_v<nadir::units::PositionMeters, nadir::math::Vec3d>);

int main() {
    using namespace nadir::units;

    if (to_meters(Kilometers{1.0}).value != 1000.0) return 1;
    if (to_kilometers(Meters{1500.0}).value != 1.5) return 2;
    if (std::abs(to_radians(Degrees{180.0}).value - std::numbers::pi) > 1.0e-14) return 3;
    if (to_seconds(Days{1.0}).value != 86400.0) return 4;
    if (to_meters_per_second(KilometersPerSecond{7.5}).value != 7500.0) return 5;
    if (to_hertz(Megahertz{145.8}).value != 145'800'000.0) return 6;

    const PositionKilometers position_km{{1.0, -2.0, 3.5}};
    const auto position_m = to_meters(position_km);
    if (position_m.value.x != 1000.0 || position_m.value.y != -2000.0 ||
        position_m.value.z != 3500.0) return 7;

    const VelocityKilometersPerSecond velocity_km_s{{7.5, 0.0, -1.0}};
    const auto velocity_m_s = to_meters_per_second(velocity_km_s);
    if (velocity_m_s.value.x != 7500.0 || velocity_m_s.value.y != 0.0 ||
        velocity_m_s.value.z != -1000.0) return 8;

    if (speed_of_light.value != 299'792'458.0) return 9;
    if (wgs84_semi_major_axis_m.value != 6'378'137.0) return 10;

    return 0;
}
