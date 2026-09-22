#pragma once

#include <nadir/math/vec3.hpp>

#include <optional>
#include <span>

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

struct ThirdBody {
    math::Vec3d position_m{}; // Body position relative to the central body.
    double mu_m3_s2{};
};

// A deliberately explicit, exponential atmosphere for scenario analysis. Its
// parameters must be supplied by the caller; it is not a validated atmosphere
// data product.
struct DragModel {
    double drag_coefficient{};
    double area_m2{};
    double mass_kg{};
    double reference_density_kg_m3{};
    double reference_altitude_m{};
    double scale_height_m{};
    double earth_rotation_rad_s{7.2921150e-5};
};

// Cannonball solar-radiation-pressure model. The caller supplies the Sun's
// inertial geocentric position and spacecraft optical properties.
struct SolarRadiationPressureModel {
    math::Vec3d sun_position_m{};
    double reflectivity_coefficient{};
    double area_m2{};
    double mass_kg{};
    double pressure_at_1au_n_m2{4.56e-6};
    double astronomical_unit_m{149'597'870'700.0};
};

struct IntegratorSettings {
    double initial_step_s{60.0};
    double minimum_step_s{0.01};
    double maximum_step_s{300.0};
    double position_tolerance_m{1.0};
};

// Earth-centered inertial acceleration: central gravity plus optional J2.
math::Vec3d gravity_acceleration(const math::Vec3d& position_m, const GravityModel& model = {},
                                 std::span<const ThirdBody> third_bodies = {});

math::Vec3d drag_acceleration(const CartesianState& state, const DragModel& drag, const GravityModel& earth = {});
math::Vec3d solar_radiation_pressure_acceleration(const CartesianState& state, const SolarRadiationPressureModel& model);

// Fixed-step fourth-order Runge-Kutta propagator. It is deliberately separate
// from SGP4, which remains the correct model for GP/OMM mean elements.
std::optional<CartesianState> propagate_numerical(CartesianState initial, double duration_s,
                                                   double step_s, const GravityModel& model = {},
                                                   std::span<const ThirdBody> third_bodies = {},
                                                   std::optional<DragModel> drag = std::nullopt,
                                                   std::optional<SolarRadiationPressureModel> solar_radiation_pressure = std::nullopt);

std::optional<CartesianState> propagate_numerical_adaptive(CartesianState initial, double duration_s,
                                                            IntegratorSettings settings = {}, const GravityModel& model = {},
                                                            std::span<const ThirdBody> third_bodies = {},
                                                            std::optional<DragModel> drag = std::nullopt,
                                                            std::optional<SolarRadiationPressureModel> solar_radiation_pressure = std::nullopt);

} // namespace nadir::orbit
