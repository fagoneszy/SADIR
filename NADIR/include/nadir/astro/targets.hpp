#pragma once
#include <optional>
#include <string>
#include <vector>

namespace nadir::astro {

struct SolarTarget {
    std::string name;
    int horizons_id{};
    std::string type;
    std::string primary;
};

const std::vector<SolarTarget>& solar_targets();
std::optional<SolarTarget> find_solar_target(const std::string& name);
std::vector<SolarTarget> search_solar_targets(const std::string& query);

}
