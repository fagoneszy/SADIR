#pragma once

#include <functional>
#include <optional>
#include <vector>

namespace nadir::orbit {

struct Pass {
    double aos_minutes{};
    double tca_minutes{};
    double los_minutes{};
    double max_elevation_deg{};
};

using ElevationFunction = std::function<std::optional<double>(double minutes_since_epoch)>;

std::vector<Pass> predict_passes(const ElevationFunction& elevation, double start_minutes,
                                 double end_minutes, double step_minutes,
                                 double minimum_elevation_deg = 0.0);

} // namespace nadir::orbit
