#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace nadir::earth {

struct Earthquake {
    std::string id;
    std::string place;
    std::string url;
    double longitude_deg{};
    double latitude_deg{};
    double depth_km{};
    double magnitude{};
    std::int64_t time_ms{};
    std::int64_t updated_ms{};
};

struct EarthquakeFeed {
    std::string title;
    std::int64_t generated_ms{};
    std::vector<Earthquake> events;
};

struct EarthquakeParseResult {
    bool ok{};
    EarthquakeFeed feed;
    std::string error;
};

EarthquakeParseResult parse_usgs_geojson(const std::string& text);
EarthquakeParseResult load_usgs_geojson(const std::string& path);

}
