#pragma once

#include <cstdint>
#include <string>

#include <nadir/math/vec3.hpp>
#include <nadir/math/quat.hpp>

namespace nadir::space {

struct BodyPhysicalParams {
    double equatorial_radius_km{};
    double mean_radius_km{};
    double mass_kg{};
    double sidereal_rotation_period_hours{};
    double obliquity_deg{};
    bool retrograde{false};
};

struct BodyOrbitalParams {
    double sidereal_orbital_period_days{};
    double orbital_inclination_deg{};
    double orbital_eccentricity{};
    double mean_distance_from_sun_km{};
    double perihelion_km{};
    double aphelion_km{};
    double orbital_velocity_km_s{};
};

struct BodyParams {
    std::string name{};
    int horizons_id{};
    BodyPhysicalParams physical{};
    BodyOrbitalParams orbital{};
};

struct SpinState {
    nadir::math::Quatd orientation{nadir::math::Quatd::identity()};
    double obliquity_deg{};
    double rotation_period_hours{};
    bool retrograde{false};
};

struct BodyState {
    std::string name{};
    int horizons_id{};
    nadir::math::Vec3d heliocentric_position_km{};
    nadir::math::Vec3d heliocentric_velocity_km_s{};
    double distance_from_sun_km{};
    std::int64_t utc_ns{};
};

}