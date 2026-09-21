#pragma once

#include <array>
#include <optional>

namespace nadir::state {

// Values use the fixed SI Cartesian ordering [x, y, z, vx, vy, vz].
enum class CovarianceConvention {
    CartesianPositionVelocitySI
};

struct Covariance6 {
    std::array<double, 36> values{};
    CovarianceConvention convention{CovarianceConvention::CartesianPositionVelocitySI};
};

struct StateUncertainty {
    bool known{};
    Covariance6 covariance{};
};

bool valid_covariance(const Covariance6& covariance, double symmetry_tolerance = 1.0e-9) noexcept;
std::optional<double> position_sigma_m(const StateUncertainty& uncertainty) noexcept;
Covariance6 rotate_covariance(const Covariance6& covariance, const std::array<double, 9>& rotation) noexcept;

} // namespace nadir::state
