#pragma once

#include <optional>
#include <string>

namespace nadir::astro {

struct CdmRecord {
    std::string tca;
    std::string object1_id;
    std::string object2_id;
    double miss_distance_m{};
    double relative_speed_m_s{};
    std::optional<double> collision_probability;
};

struct CdmParseResult {
    bool ok{};
    std::optional<CdmRecord> record;
    std::string error;
};

// Parses the conjunction-summary subset of one CCSDS CDM KVN message.
// MISS_DISTANCE and RELATIVE_SPEED are km and km/s on the wire.
CdmParseResult parse_cdm_kvn(const std::string& text);

} // namespace nadir::astro
