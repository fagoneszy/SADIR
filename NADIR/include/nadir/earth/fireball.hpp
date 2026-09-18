#pragma once
#include <optional>
#include <string>
#include <vector>

namespace nadir::earth {

struct Fireball {
    std::string date;
    std::optional<double> latitude_deg;
    std::optional<double> longitude_deg;
    std::optional<double> altitude_km;
    std::optional<double> velocity_km_s;
    std::optional<double> radiated_energy_1e10_j;
    std::optional<double> impact_energy_kt;
};

struct FireballFeed {
    std::string version;
    std::vector<Fireball> events;
};

std::optional<FireballFeed> parse_jpl_fireballs(const std::string& text);

}
