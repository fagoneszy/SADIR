#pragma once

#include <nadir/orbit/numerical.hpp>

#include <optional>
#include <string>

namespace nadir::astro {

struct OpmRecord {
    std::string object_name;
    std::string object_id;
    std::string epoch;
    std::string center_name;
    std::string ref_frame;
    std::string time_system;
    orbit::CartesianState state_m;
};

struct OpmParseResult {
    bool ok{};
    std::optional<OpmRecord> record;
    std::string error;
};

// Parses a single CCSDS OPM KVN message. Cartesian X/Y/Z are km and
// X_DOT/Y_DOT/Z_DOT are km/s on the wire, normalized to SI in the result.
OpmParseResult parse_opm_kvn(const std::string& text);
std::string write_opm_kvn(const OpmRecord& record);

} // namespace nadir::astro
