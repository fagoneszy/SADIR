#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace nadir::astro {

struct OmmRecord {
    std::string object_name;
    std::string object_id;
    std::string epoch;
    std::string classification_type;
    std::string center_name;
    std::string ref_frame;
    std::string time_system;
    std::string mean_element_theory;
    std::uint64_t norad_cat_id{};
    int ephemeris_type{};
    int element_set_no{};
    int rev_at_epoch{};
    double mean_motion_rev_day{};
    double eccentricity{};
    double inclination_deg{};
    double raan_deg{};
    double arg_pericenter_deg{};
    double mean_anomaly_deg{};
    double bstar{};
    double mean_motion_dot{};
    double mean_motion_ddot{};
};

struct OmmParseResult {
    bool ok{};
    std::vector<OmmRecord> records;
    std::string error;
};

OmmParseResult parse_omm_json(const std::string& text);
OmmParseResult load_omm_json(const std::string& path);
std::vector<OmmRecord> find_omm(const std::vector<OmmRecord>& records,const std::string& query,std::size_t limit=50);

}
