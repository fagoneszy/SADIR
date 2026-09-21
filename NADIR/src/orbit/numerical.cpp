#include <nadir/orbit/numerical.hpp>

#include <algorithm>
#include <cmath>

namespace nadir::orbit {
namespace {
bool valid_model(const GravityModel& model) {
    return std::isfinite(model.mu_m3_s2) && std::isfinite(model.equatorial_radius_m) && std::isfinite(model.j2) &&
           model.mu_m3_s2 > 0.0 && model.equatorial_radius_m > 0.0;
}
CartesianState derivative(const CartesianState& state, const GravityModel& model, std::span<const ThirdBody> third_bodies) {
    return {state.velocity_m_s, gravity_acceleration(state.position_m, model, third_bodies)};
}
CartesianState add_scaled(const CartesianState& state, const CartesianState& slope, double scale) {
    return {state.position_m + slope.position_m * scale, state.velocity_m_s + slope.velocity_m_s * scale};
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

std::optional<CartesianState> propagate_numerical(CartesianState state, double duration_s,
                                                    double step_s, const GravityModel& model, std::span<const ThirdBody> third_bodies) {
    if (!valid_model(model) || !std::isfinite(duration_s) || !std::isfinite(step_s) || step_s <= 0.0) return std::nullopt;
    const double direction = duration_s < 0.0 ? -1.0 : 1.0;
    double remaining = std::abs(duration_s);
    while (remaining > 0.0) {
        const double h = direction * std::min(step_s, remaining);
        const auto k1 = derivative(state, model, third_bodies);
        const auto k2 = derivative(add_scaled(state, k1, h * 0.5), model, third_bodies);
        const auto k3 = derivative(add_scaled(state, k2, h * 0.5), model, third_bodies);
        const auto k4 = derivative(add_scaled(state, k3, h), model, third_bodies);
        state.position_m = state.position_m + (k1.position_m + 2.0 * k2.position_m + 2.0 * k3.position_m + k4.position_m) * (h / 6.0);
        state.velocity_m_s = state.velocity_m_s + (k1.velocity_m_s + 2.0 * k2.velocity_m_s + 2.0 * k3.velocity_m_s + k4.velocity_m_s) * (h / 6.0);
        if (!std::isfinite(state.position_m.x) || !std::isfinite(state.position_m.y) || !std::isfinite(state.position_m.z)) return std::nullopt;
        remaining -= std::abs(h);
    }
    return state;
}

} // namespace nadir::orbit
