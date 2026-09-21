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
    if (orbit::gravity_acceleration({radius, 0.0, 0.0}, central).norm() < 8.0) return 2;
    const orbit::ThirdBody distant{{384'400'000.0, 0.0, 0.0}, 4.9048695e12};
    const std::array bodies{distant};
    const auto differential = orbit::gravity_acceleration({radius, 0.0, 0.0}, central, bodies);
    if (!std::isfinite(differential.x) || std::abs(differential.x - orbit::gravity_acceleration({radius, 0.0, 0.0}, central).x) < 1.0e-12) return 3;
    return orbit::propagate_numerical(initial, 1.0, 0.0, central) ? 4 : 0;
}
