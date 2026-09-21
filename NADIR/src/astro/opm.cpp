#include <nadir/astro/opm.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <map>
#include <sstream>

namespace nadir::astro {
namespace {
std::string trim(std::string value) {
    const auto first = value.find_first_not_of(" \t\r");
    const auto last = value.find_last_not_of(" \t\r");
    return first == std::string::npos ? std::string{} : value.substr(first, last - first + 1);
}
std::optional<double> number(const std::map<std::string, std::string>& fields, const char* key) {
    const auto it = fields.find(key);
    if (it == fields.end()) return std::nullopt;
    try {
        std::size_t consumed{}; const double value = std::stod(it->second, &consumed);
        return consumed > 0 && std::isfinite(value) ? std::optional{value} : std::nullopt;
    } catch (...) { return std::nullopt; }
}
} // namespace

OpmParseResult parse_opm_kvn(const std::string& text) {
    std::map<std::string, std::string> fields;
    std::istringstream input(text);
    for (std::string line; std::getline(input, line); ) {
        const auto equals = line.find('=');
        if (equals == std::string::npos) continue;
        auto key = trim(line.substr(0, equals));
        key.erase(std::remove_if(key.begin(), key.end(), [](unsigned char c) { return std::isspace(c); }), key.end());
        fields[key] = trim(line.substr(equals + 1));
    }
    const auto required = [&](const char* key) { return fields.contains(key) && !fields[key].empty(); };
    if (!required("CCSDS_OPM_VERS") || !required("OBJECT_NAME") || !required("EPOCH") || !required("REF_FRAME") || !required("TIME_SYSTEM")) return {false, {}, "missing OPM metadata"};
    const auto x=number(fields,"X"), y=number(fields,"Y"), z=number(fields,"Z");
    const auto vx=number(fields,"X_DOT"), vy=number(fields,"Y_DOT"), vz=number(fields,"Z_DOT");
    if (!x || !y || !z || !vx || !vy || !vz) return {false, {}, "missing OPM state"};
    OpmRecord record;
    record.object_name=fields["OBJECT_NAME"]; record.object_id=fields["OBJECT_ID"]; record.epoch=fields["EPOCH"];
    record.center_name=fields["CENTER_NAME"]; record.ref_frame=fields["REF_FRAME"]; record.time_system=fields["TIME_SYSTEM"];
    record.state_m={{*x*1000.0,*y*1000.0,*z*1000.0},{*vx*1000.0,*vy*1000.0,*vz*1000.0}};
    return {true, std::move(record), {}};
}

} // namespace nadir::astro
