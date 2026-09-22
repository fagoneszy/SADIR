#include <nadir/astro/tdm.hpp>

#include <cmath>
#include <sstream>

namespace nadir::astro {
namespace {
constexpr std::size_t maximum_message_bytes = 1024U * 1024U;
constexpr std::size_t maximum_lines = 4096U;

std::string trim(std::string value) {
    const auto first = value.find_first_not_of(" \t\r");
    const auto last = value.find_last_not_of(" \t\r");
    return first == std::string::npos ? std::string{} : value.substr(first, last - first + 1);
}

std::optional<double> number(const std::string& text) {
    try {
        std::istringstream input(text);
        double value{};
        std::string unit;
        if (!(input >> value) || !std::isfinite(value) || (input >> unit && !unit.starts_with("["))) return std::nullopt;
        return value;
    } catch (...) { return std::nullopt; }
}
} // namespace

TdmParseResult parse_tdm_kvn(const std::string& text) {
    if (text.size() > maximum_message_bytes) return {false, {}, "TDM message exceeds size limit"};
    TdmRecord record;
    bool in_data{};
    bool version_seen{};
    std::size_t line_count{};
    std::istringstream input(text);
    for (std::string line; std::getline(input, line); ) {
        if (++line_count > maximum_lines) return {false, {}, "TDM message has too many lines"};
        line = trim(std::move(line));
        if (line.empty() || line.starts_with("COMMENT")) continue;
        if (line == "DATA_START") { if (in_data) return {false, {}, "invalid TDM data section"}; in_data = true; continue; }
        if (line == "DATA_STOP") { if (!in_data) return {false, {}, "invalid TDM data stop"}; in_data = false; continue; }
        const auto equals = line.find('=');
        if (equals == std::string::npos) continue;
        const auto key = trim(line.substr(0, equals));
        const auto value = trim(line.substr(equals + 1));
        if (!in_data) {
            if (key == "CCSDS_TDM_VERS") version_seen = !value.empty();
            if (key == "TIME_SYSTEM") record.time_system = value;
            continue;
        }
        if (key == "EPOCH") {
            if (value.empty()) return {false, {}, "empty TDM epoch"};
            record.observations.push_back({value, {}, {}, {}});
            continue;
        }
        if (record.observations.empty()) return {false, {}, "TDM measurement without epoch"};
        auto& observation = record.observations.back();
        const auto measured = number(value);
        if ((key == "RANGE" || key == "RANGE_RATE" || key == "DOPPLER_INSTANTANEOUS") && !measured)
            return {false, {}, "invalid TDM measurement"};
        if (key == "RANGE") {
            if (observation.range_m) return {false, {}, "duplicate TDM range"};
            observation.range_m = *measured * 1000.0;
        } else if (key == "RANGE_RATE") {
            if (observation.range_rate_m_s) return {false, {}, "duplicate TDM range rate"};
            observation.range_rate_m_s = *measured * 1000.0;
        } else if (key == "DOPPLER_INSTANTANEOUS") {
            if (observation.doppler_hz) return {false, {}, "duplicate TDM Doppler"};
            observation.doppler_hz = *measured;
        }
    }
    if (in_data || !version_seen || record.time_system.empty() || record.observations.empty()) return {false, {}, "missing TDM metadata or data"};
    for (const auto& observation : record.observations)
        if (!observation.range_m && !observation.range_rate_m_s && !observation.doppler_hz)
            return {false, {}, "TDM epoch has no supported measurement"};
    return {true, std::move(record), {}};
}

std::string write_tdm_kvn(const TdmRecord& record) {
    if (record.time_system.empty() || record.observations.empty()) return {};
    std::ostringstream out;
    out.precision(17);
    out << "CCSDS_TDM_VERS = 2.0\nTIME_SYSTEM = " << record.time_system << "\nDATA_START\n";
    for (const auto& observation : record.observations) {
        if (observation.epoch.empty() || (!observation.range_m && !observation.range_rate_m_s && !observation.doppler_hz)) return {};
        out << "EPOCH = " << observation.epoch << '\n';
        if (observation.range_m) out << "RANGE = " << *observation.range_m / 1000.0 << " [km]\n";
        if (observation.range_rate_m_s) out << "RANGE_RATE = " << *observation.range_rate_m_s / 1000.0 << " [km/s]\n";
        if (observation.doppler_hz) out << "DOPPLER_INSTANTANEOUS = " << *observation.doppler_hz << " [Hz]\n";
    }
    out << "DATA_STOP\n";
    return out.str();
}

} // namespace nadir::astro
