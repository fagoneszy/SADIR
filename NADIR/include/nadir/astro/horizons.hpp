#pragma once
#include <nadir/core/math.hpp>
#include <optional>
#include <string>

namespace nadir::astro {

struct EphemerisState {
    double jd{};
    Vec3 position_km{};
    Vec3 velocity_km_s{};
    double light_time_s{};
    double range_km{};
    double range_rate_km_s{};
};

std::optional<EphemerisState> parse_horizons_vector_csv(const std::string& response);

}
