#include <nadir/satellite/tle.hpp>
#include <charconv>
#include <cstdlib>

namespace nadir::satellite {

static double number(const std::string& s, std::size_t start, std::size_t count) {
    return std::strtod(s.substr(start, count).c_str(), nullptr);
}

static int integer(const std::string& s, std::size_t start, std::size_t count) {
    return static_cast<int>(std::strtol(s.substr(start, count).c_str(), nullptr, 10));
}

std::optional<Tle> parse_tle(const std::string& name, const std::string& l1, const std::string& l2) {
    if (l1.size() < 32 || l2.size() < 63 || l1[0] != '1' || l2[0] != '2') return std::nullopt;
    Tle t{};
    t.name = name;
    t.norad_id = integer(l1, 2, 5);
    t.epoch_year = integer(l1, 18, 2);
    t.epoch_day = number(l1, 20, 12);
    t.inclination_deg = number(l2, 8, 8);
    t.raan_deg = number(l2, 17, 8);
    t.eccentricity = number("0." + l2.substr(26, 7), 0, 9);
    t.argument_perigee_deg = number(l2, 34, 8);
    t.mean_anomaly_deg = number(l2, 43, 8);
    t.mean_motion_rev_per_day = number(l2, 52, 11);
    return t;
}

}
