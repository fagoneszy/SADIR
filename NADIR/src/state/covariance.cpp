#include <nadir/state/covariance.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

namespace nadir::state {
bool valid_covariance(const Covariance6& covariance, double tolerance) noexcept {
    if (!std::isfinite(tolerance) || tolerance < 0.0) return false;
    for (int row = 0; row < 6; ++row) {
        if (!std::isfinite(covariance.values[row * 6 + row]) || covariance.values[row * 6 + row] < 0.0) return false;
        for (int col = row + 1; col < 6; ++col) {
            const auto a = covariance.values[row * 6 + col], b = covariance.values[col * 6 + row];
            if (!std::isfinite(a) || !std::isfinite(b) || std::abs(a - b) > tolerance) return false;
        }
    }
    return true;
}

std::optional<double> position_sigma_m(const StateUncertainty& uncertainty) noexcept {
    if (!uncertainty.known || !valid_covariance(uncertainty.covariance)) return std::nullopt;
    const auto& c = uncertainty.covariance.values;
    return std::sqrt(std::max({c[0], c[7], c[14]}));
}

Covariance6 rotate_covariance(const Covariance6& covariance, const std::array<double, 9>& rotation) noexcept {
    Covariance6 out{}; out.convention = covariance.convention;
    double transform[36]{};
    for (int block = 0; block < 2; ++block) for (int row = 0; row < 3; ++row) for (int col = 0; col < 3; ++col)
        transform[(block * 3 + row) * 6 + block * 3 + col] = rotation[row * 3 + col];
    double temp[36]{};
    for (int r = 0; r < 6; ++r) for (int c = 0; c < 6; ++c) for (int k = 0; k < 6; ++k) temp[r * 6 + c] += transform[r * 6 + k] * covariance.values[k * 6 + c];
    for (int r = 0; r < 6; ++r) for (int c = 0; c < 6; ++c) for (int k = 0; k < 6; ++k) out.values[r * 6 + c] += temp[r * 6 + k] * transform[c * 6 + k];
    return out;
}
} // namespace nadir::state
