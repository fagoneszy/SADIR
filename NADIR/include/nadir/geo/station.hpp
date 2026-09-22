#pragma once

#include <nadir/geo/wgs84.hpp>

#include <optional>
#include <string>
#include <vector>

namespace nadir::geo {

struct GroundStation {
    std::string id;
    std::string name;
    Geodetic location{};
    double elevation_mask_deg{};
    std::string source;

    [[nodiscard]] bool valid() const noexcept;
};

// TSV schema: id, name, latitude_deg, longitude_deg, altitude_m,
// elevation_mask_deg, source. Header is required; malformed rows are ignored.
std::optional<std::vector<GroundStation>> load_ground_stations_tsv(const std::string& path);
std::optional<std::vector<GroundStation>> parse_noaa_coops_stations_json(const std::string& text);
const GroundStation* find_ground_station(const std::vector<GroundStation>& stations,
                                        const std::string& id) noexcept;

} // namespace nadir::geo
