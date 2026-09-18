#pragma once
#include <optional>
#include <string>

namespace nadir::satellite {

struct Tle {
    std::string name;
    int norad_id{};
    int epoch_year{};
    double epoch_day{};
    double inclination_deg{};
    double raan_deg{};
    double eccentricity{};
    double argument_perigee_deg{};
    double mean_anomaly_deg{};
    double mean_motion_rev_per_day{};
};

std::optional<Tle> parse_tle(const std::string& name, const std::string& line1, const std::string& line2);

}
