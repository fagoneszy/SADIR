#include <nadir/geo/station.hpp>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

namespace nadir::geo {
namespace {
std::vector<std::string> split_tab(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    std::istringstream input(line);
    while (std::getline(input, field, '\t')) fields.push_back(std::move(field));
    return fields;
}
std::optional<double> number(const std::string& text) {
    try {
        std::size_t consumed{};
        const double value = std::stod(text, &consumed);
        return consumed == text.size() && std::isfinite(value) ? std::optional{value} : std::nullopt;
    } catch (...) { return std::nullopt; }
}
} // namespace

bool GroundStation::valid() const noexcept {
    return !id.empty() && std::isfinite(location.latitude_deg) && std::isfinite(location.longitude_deg) &&
           std::isfinite(location.altitude_m) && std::isfinite(elevation_mask_deg) &&
           location.latitude_deg >= -90.0 && location.latitude_deg <= 90.0 &&
           location.longitude_deg >= -180.0 && location.longitude_deg <= 180.0 &&
           elevation_mask_deg >= -90.0 && elevation_mask_deg <= 90.0;
}

std::optional<std::vector<GroundStation>> load_ground_stations_tsv(const std::string& path) {
    std::ifstream input(path);
    if (!input) return std::nullopt;
    std::string line;
    if (!std::getline(input, line) || split_tab(line) != std::vector<std::string>{"id", "name", "latitude_deg", "longitude_deg", "altitude_m", "elevation_mask_deg", "source"}) return std::nullopt;
    std::vector<GroundStation> stations;
    while (std::getline(input, line)) {
        const auto fields = split_tab(line);
        if (fields.size() != 7) continue;
        const auto lat = number(fields[2]); const auto lon = number(fields[3]);
        const auto altitude = number(fields[4]); const auto mask = number(fields[5]);
        if (!lat || !lon || !altitude || !mask) continue;
        GroundStation station{fields[0], fields[1], {*lat, *lon, *altitude}, *mask, fields[6]};
        if (station.valid() && !find_ground_station(stations, station.id)) stations.push_back(std::move(station));
    }
    return stations;
}

const GroundStation* find_ground_station(const std::vector<GroundStation>& stations, const std::string& id) noexcept {
    const auto found = std::find_if(stations.begin(), stations.end(), [&](const auto& station) { return station.id == id; });
    return found == stations.end() ? nullptr : &*found;
}

} // namespace nadir::geo
