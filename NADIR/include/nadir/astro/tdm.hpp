#pragma once

#include <optional>
#include <string>
#include <vector>

namespace nadir::astro {

struct TdmObservation {
    std::string epoch;
    std::optional<double> range_m;
    std::optional<double> range_rate_m_s;
    std::optional<double> doppler_hz;
};

struct TdmRecord {
    std::string time_system;
    std::vector<TdmObservation> observations;
};

struct TdmParseResult {
    bool ok{};
    std::optional<TdmRecord> record;
    std::string error;
};

// Parses the range/range-rate/instantaneous-Doppler subset of one CCSDS TDM
// KVN message. RANGE is km and RANGE_RATE is km/s on the wire.
TdmParseResult parse_tdm_kvn(const std::string& text);
std::string write_tdm_kvn(const TdmRecord& record);

} // namespace nadir::astro
