#include <nadir/orbit/numerical.hpp>

#include <cmath>
#include <array>

int main() {
    using namespace nadir;
    orbit::GravityModel central{}; central.j2 = 0.0;
    constexpr double radius = 7'000'000.0;
    const double velocity = std::sqrt(central.mu_m3_s2 / radius);
    const orbit::CartesianState initial{{radius, 0.0, 0.0}, {0.0, velocity, 0.0}};
    const double seconds = 60.0;
    const auto propagated = orbit::propagate_numerical(initial, seconds, 1.0, central);
    const double angular_rate = velocity / radius;
    const math::Vec3d expected{radius * std::cos(angular_rate * seconds), radius * std::sin(angular_rate * seconds), 0.0};
    if (!propagated || (propagated->position_m - expected).norm() > 0.2) return 1;
    const auto adaptive=orbit::propagate_numerical_adaptive(initial, seconds, {.initial_step_s=60.0,.minimum_step_s=0.01,.maximum_step_s=60.0,.position_tolerance_m=0.01}, central);
    if (!adaptive || (adaptive->position_m - expected).norm() > 0.2) return 2;
    if (orbit::gravity_acceleration({radius, 0.0, 0.0}, central).norm() < 8.0) return 3;
    const auto with_j2 = orbit::gravity_acceleration({radius, 0.0, 0.0}, orbit::GravityModel{});
    const auto without_j2 = orbit::gravity_acceleration({radius, 0.0, 0.0}, central);
    if (!(std::abs(with_j2.x - without_j2.x) > 1.0e-3)) return 4;
    const orbit::ThirdBody distant{{384'400'000.0, 0.0, 0.0}, 4.9048695e12};
    const std::array bodies{distant};
    const auto differential = orbit::gravity_acceleration({radius, 0.0, 0.0}, central, bodies);
    if (!std::isfinite(differential.x) || std::abs(differential.x - orbit::gravity_acceleration({radius, 0.0, 0.0}, central).x) < 1.0e-12) return 5;
    const orbit::DragModel drag{.drag_coefficient=2.2, .area_m2=10.0, .mass_kg=500.0,
                                 .reference_density_kg_m3=1.0e-9, .reference_altitude_m=200'000.0,
                                 .scale_height_m=50'000.0};
    const orbit::CartesianState low_orbit{{6'578'137.0, 0.0, 0.0}, {0.0, 7'800.0, 0.0}};
    const auto drag_a = orbit::drag_acceleration(low_orbit, drag, central);
    if (!(drag_a.y < 0.0) || !std::isfinite(drag_a.y)) return 6;
    const auto with_drag = orbit::propagate_numerical(low_orbit, 60.0, 1.0, central, {}, drag);
    const auto without_drag = orbit::propagate_numerical(low_orbit, 60.0, 1.0, central);
    if (!with_drag || !without_drag || with_drag->velocity_m_s.norm() >= without_drag->velocity_m_s.norm()) return 7;
    const orbit::SolarRadiationPressureModel srp{.sun_position_m={149'597'870'700.0, 0.0, 0.0}, .reflectivity_coefficient=1.2, .area_m2=10.0, .mass_kg=500.0};
    const auto srp_a = orbit::solar_radiation_pressure_acceleration(low_orbit, srp);
    if (!(srp_a.x < 0.0) || !std::isfinite(srp_a.x)) return 8;
    return orbit::propagate_numerical(initial, 1.0, 0.0, central) ? 9 : 0;
}
