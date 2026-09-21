#include <nadir/orbit/pass_predictor.hpp>
#include <cmath>
int main() {
    const auto elevation = [](double t) -> std::optional<double> { return 30.0 - (t - 10.0) * (t - 10.0); };
    const auto passes = nadir::orbit::predict_passes(elevation, 0.0, 20.0, 1.0, 0.0);
    if (passes.size() != 1) return 1;
    const auto& pass = passes[0];
    if (std::abs(pass.aos_minutes - (10.0 - std::sqrt(30.0))) > 1e-8 ||
        std::abs(pass.los_minutes - (10.0 + std::sqrt(30.0))) > 1e-8 ||
        std::abs(pass.tca_minutes - 10.0) > 1.0 || pass.max_elevation_deg < 29.0) return 2;
    return nadir::orbit::predict_passes(elevation, 2.0, 1.0, 1.0).empty() ? 0 : 3;
}
