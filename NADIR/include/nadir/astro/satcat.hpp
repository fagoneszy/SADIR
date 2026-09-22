#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace nadir::astro {

// SATCAT fields are deliberately separate from GP/OMM elements: they describe
// catalog identity and status, not an orbital state vector.
struct SatcatRecord {
    std::uint64_t norad_cat_id{};
    std::string object_name;
    std::string object_id;
    std::string object_type;
    std::string ops_status_code;
    std::string owner;
    std::string launch_date;
    std::string decay_date;
    std::string data_status_code;
    std::string orbit_center;
    std::string orbit_type;
};

struct SatcatParseResult { bool ok{}; std::vector<SatcatRecord> records; std::string error; };
SatcatParseResult parse_satcat_json(const std::string& text);

} // namespace nadir::astro
