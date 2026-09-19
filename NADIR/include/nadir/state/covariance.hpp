#pragma once

#include <array>

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

} // namespace nadir::state
