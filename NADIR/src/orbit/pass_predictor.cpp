#include <nadir/orbit/pass_predictor.hpp>

#include <algorithm>
#include <cmath>

namespace nadir::orbit {
namespace {
double crossing(const ElevationFunction& elevation, double a, double b, double mask) {
    for (int i = 0; i < 48; ++i) {
        const double mid = (a + b) * 0.5;
        const auto ea = elevation(a); const auto em = elevation(mid);
        if (!ea || !em) return mid;
        if ((*ea >= mask) == (*em >= mask)) a = mid; else b = mid;
    }
    return (a + b) * 0.5;
}
}

std::vector<Pass> predict_passes(const ElevationFunction& elevation, double start, double end,
                                 double step, double mask) {
    std::vector<Pass> passes;
    if (!elevation || !std::isfinite(start) || !std::isfinite(end) || !std::isfinite(step) ||
        end <= start || step <= 0.0) return passes;
    bool active = false;
    Pass current{};
    auto previous = elevation(start);
    for (double t = start + step; t <= end + step * 0.5; t += step) {
        const double sample_t = std::min(t, end);
        const auto value = elevation(sample_t);
        if (!previous || !value) { previous = value; continue; }
        const bool was_above = *previous >= mask;
        const bool above = *value >= mask;
        if (!active && (!was_above && above || (t == start + step && was_above))) {
            current.aos_minutes = was_above ? start : crossing(elevation, sample_t - step, sample_t, mask);
            current.tca_minutes = current.aos_minutes;
            current.max_elevation_deg = std::max(*previous, *value);
            active = true;
        }
        if (active) {
            if (*value > current.max_elevation_deg) { current.max_elevation_deg = *value; current.tca_minutes = sample_t; }
            if (was_above && !above) {
                current.los_minutes = crossing(elevation, sample_t - step, sample_t, mask);
                passes.push_back(current); active = false;
            }
        }
        previous = value;
    }
    return passes;
}
} // namespace nadir::orbit
