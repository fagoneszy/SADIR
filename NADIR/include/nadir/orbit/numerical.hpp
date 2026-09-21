#pragma once

#include <nadir/math/vec3.hpp>

#include <optional>

namespace nadir::orbit {

struct CartesianState {
    math::Vec3d position_m{};
    math::Vec3d velocity_m_s{};
};

struct GravityModel {
    double mu_m3_s2{3.986004418e14};
    double equatorial_radius_m{6'378'137.0};
    double j2{1.08262668e-3};
};

// Earth-centered inertial acceleration: central gravity plus optional J2.
math::Vec3d gravity_acceleration(const math::Vec3d& position_m, const GravityModel& model = {});

// Fixed-step fourth-order Runge-Kutta propagator. It is deliberately separate
// from SGP4, which remains the correct model for GP/OMM mean elements.
std::optional<CartesianState> propagate_numerical(CartesianState initial, double duration_s,
                                                   double step_s, const GravityModel& model = {});

} // namespace nadir::orbit
