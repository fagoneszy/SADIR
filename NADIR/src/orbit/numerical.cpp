#include <nadir/orbit/numerical.hpp>

#include <algorithm>
#include <cmath>

namespace nadir::orbit {
namespace {
bool valid_model(const GravityModel& model) {
    return std::isfinite(model.mu_m3_s2) && std::isfinite(model.equatorial_radius_m) && std::isfinite(model.j2) &&
           model.mu_m3_s2 > 0.0 && model.equatorial_radius_m > 0.0;
}
bool valid_drag(const DragModel& drag) {
    return std::isfinite(drag.drag_coefficient) && std::isfinite(drag.area_m2) && std::isfinite(drag.mass_kg) &&
           std::isfinite(drag.reference_density_kg_m3) && std::isfinite(drag.reference_altitude_m) &&
           std::isfinite(drag.scale_height_m) && std::isfinite(drag.earth_rotation_rad_s) &&
           drag.drag_coefficient >= 0.0 && drag.area_m2 >= 0.0 && drag.mass_kg > 0.0 &&
           drag.reference_density_kg_m3 >= 0.0 && drag.scale_height_m > 0.0;
}
CartesianState derivative(const CartesianState& state, const GravityModel& model, std::span<const ThirdBody> third_bodies,
                          const std::optional<DragModel>& drag) {
    auto acceleration = gravity_acceleration(state.position_m, model, third_bodies);
    if (drag) acceleration += drag_acceleration(state, *drag, model);
    return {state.velocity_m_s, acceleration};
}
CartesianState add_scaled(const CartesianState& state, const CartesianState& slope, double scale) {
    return {state.position_m + slope.position_m * scale, state.velocity_m_s + slope.velocity_m_s * scale};
}
CartesianState rk4_step(CartesianState state, double h, const GravityModel& model, std::span<const ThirdBody> bodies,
                        const std::optional<DragModel>& drag) {
    const auto k1 = derivative(state, model, bodies, drag);
    const auto k2 = derivative(add_scaled(state, k1, h * 0.5), model, bodies, drag);
    const auto k3 = derivative(add_scaled(state, k2, h * 0.5), model, bodies, drag);
    const auto k4 = derivative(add_scaled(state, k3, h), model, bodies, drag);
    state.position_m = state.position_m + (k1.position_m + 2.0*k2.position_m + 2.0*k3.position_m + k4.position_m) * (h/6.0);
    state.velocity_m_s = state.velocity_m_s + (k1.velocity_m_s + 2.0*k2.velocity_m_s + 2.0*k3.velocity_m_s + k4.velocity_m_s) * (h/6.0);
    return state;
}
} // namespace

math::Vec3d gravity_acceleration(const math::Vec3d& r, const GravityModel& model, std::span<const ThirdBody> third_bodies) {
    const double r2 = r.norm_sq();
    if (!valid_model(model) || !std::isfinite(r2) || r2 <= 0.0) return {};
    const double distance = std::sqrt(r2);
    const double base = -model.mu_m3_s2 / (r2 * distance);
    const double z2_over_r2 = r.z * r.z / r2;
    const double j2_factor = 1.5 * model.j2 * (model.equatorial_radius_m * model.equatorial_radius_m / r2);
    math::Vec3d acceleration{base * r.x * (1.0 + j2_factor * (1.0 - 5.0 * z2_over_r2)),
                              base * r.y * (1.0 + j2_factor * (1.0 - 5.0 * z2_over_r2)),
                              base * r.z * (1.0 + j2_factor * (3.0 - 5.0 * z2_over_r2))};
    for (const auto& body : third_bodies) {
        const auto relative = body.position_m - r;
        const double relative_norm = relative.norm();
        const double origin_norm = body.position_m.norm();
        if (!std::isfinite(body.mu_m3_s2) || body.mu_m3_s2 <= 0.0 || relative_norm <= 0.0 || origin_norm <= 0.0) continue;
        acceleration += relative * (body.mu_m3_s2 / (relative_norm * relative_norm * relative_norm));
        acceleration -= body.position_m * (body.mu_m3_s2 / (origin_norm * origin_norm * origin_norm));
    }
    return acceleration;
}

math::Vec3d drag_acceleration(const CartesianState& state, const DragModel& drag, const GravityModel& earth) {
    if (!valid_model(earth) || !valid_drag(drag)) return {};
    const auto radius = state.position_m.norm();
    if (!std::isfinite(radius) || radius <= 0.0) return {};
    const auto altitude = radius - earth.equatorial_radius_m;
    const auto exponent = -(altitude - drag.reference_altitude_m) / drag.scale_height_m;
    if (exponent > 700.0) return {};
    const auto density = drag.reference_density_kg_m3 * std::exp(std::max(exponent, -700.0));
    const math::Vec3d atmosphere_velocity{-drag.earth_rotation_rad_s * state.position_m.y,
                                          drag.earth_rotation_rad_s * state.position_m.x, 0.0};
    const auto relative_velocity = state.velocity_m_s - atmosphere_velocity;
    const auto speed = relative_velocity.norm();
    if (!std::isfinite(density) || !std::isfinite(speed)) return {};
    return relative_velocity * (-0.5 * density * drag.drag_coefficient * drag.area_m2 * speed / drag.mass_kg);
}

std::optional<CartesianState> propagate_numerical(CartesianState state, double duration_s,
                                                    double step_s, const GravityModel& model, std::span<const ThirdBody> third_bodies,
                                                    std::optional<DragModel> drag) {
    if (!valid_model(model) || !std::isfinite(duration_s) || !std::isfinite(step_s) || step_s <= 0.0 ||
        (drag && !valid_drag(*drag))) return std::nullopt;
    const double direction = duration_s < 0.0 ? -1.0 : 1.0;
    double remaining = std::abs(duration_s);
    while (remaining > 0.0) {
        const double h = direction * std::min(step_s, remaining);
        state = rk4_step(state, h, model, third_bodies, drag);
        if (!std::isfinite(state.position_m.x) || !std::isfinite(state.position_m.y) || !std::isfinite(state.position_m.z)) return std::nullopt;
        remaining -= std::abs(h);
    }
    return state;
}

std::optional<CartesianState> propagate_numerical_adaptive(CartesianState state, double duration_s,
                                                            IntegratorSettings settings, const GravityModel& model,
                                                            std::span<const ThirdBody> third_bodies, std::optional<DragModel> drag) {
    if (!valid_model(model) || !std::isfinite(duration_s) || !std::isfinite(settings.initial_step_s) ||
        settings.minimum_step_s <= 0.0 || settings.maximum_step_s < settings.minimum_step_s ||
        settings.position_tolerance_m <= 0.0 || (drag && !valid_drag(*drag))) return std::nullopt;
    const double direction=duration_s < 0.0 ? -1.0 : 1.0;
    double remaining=std::abs(duration_s);
    double step=std::clamp(settings.initial_step_s, settings.minimum_step_s, settings.maximum_step_s);
    while (remaining > 0.0) {
        const double magnitude=std::min(step, remaining);
        const double h=direction*magnitude;
        const auto full=rk4_step(state,h,model,third_bodies,drag);
        const auto half=rk4_step(rk4_step(state,h*0.5,model,third_bodies,drag),h*0.5,model,third_bodies,drag);
        const double error=(half.position_m-full.position_m).norm()/15.0;
        if (!std::isfinite(error)) return std::nullopt;
        if (error <= settings.position_tolerance_m || magnitude <= settings.minimum_step_s) {
            state=half; remaining-=magnitude;
            step=std::min(settings.maximum_step_s, magnitude*(error <= 1e-15 ? 2.0 : std::clamp(0.9*std::pow(settings.position_tolerance_m/error,0.2),1.1,2.0)));
        } else step=std::max(settings.minimum_step_s, magnitude*std::clamp(0.9*std::pow(settings.position_tolerance_m/error,0.2),0.1,0.8));
    }
    return state;
}

} // namespace nadir::orbit
