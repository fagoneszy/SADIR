#pragma once

#include <nadir/orbit/numerical.hpp>

#include <optional>
#include <string>
#include <vector>

namespace nadir::astro {

struct OemStateSample {
    std::string epoch;
    orbit::CartesianState state_m;
    bool operator==(const OemStateSample& rhs) const noexcept {
        return epoch == rhs.epoch && state_m.position_m.x == rhs.state_m.position_m.x &&
            state_m.position_m.y == rhs.state_m.position_m.y && state_m.position_m.z == rhs.state_m.position_m.z &&
            state_m.velocity_m_s.x == rhs.state_m.velocity_m_s.x && state_m.velocity_m_s.y == rhs.state_m.velocity_m_s.y &&
            state_m.velocity_m_s.z == rhs.state_m.velocity_m_s.z;
    }
};

struct OemRecord {
    std::string object_name;
    std::string object_id;
    std::string center_name;
    std::string ref_frame;
    std::string time_system;
    std::vector<OemStateSample> samples;
};

struct OemParseResult {
    bool ok{};
    std::optional<OemRecord> record;
    std::string error;
};

// Parses one CCSDS OEM KVN segment. Data positions are km and velocities are
// km/s on the wire, normalized to SI in the result.
OemParseResult parse_oem_kvn(const std::string& text);
std::string write_oem_kvn(const OemRecord& record);

} // namespace nadir::astro
