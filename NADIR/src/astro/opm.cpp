#include <nadir/astro/opm.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <map>
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
    if (text.size() > maximum_message_bytes) return {false, {}, "OPM message exceeds size limit"};
    std::map<std::string, std::string> fields;
    std::istringstream input(text);
    std::size_t line_count{};
    for (std::string line; std::getline(input, line); ) {
        if (++line_count > maximum_lines) return {false, {}, "OPM message has too many lines"};
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

std::string write_opm_kvn(const OpmRecord& record) {
    std::ostringstream out;
    out.precision(17);
    const auto field=[&](const char* key, const std::string& value) { if (!value.empty()) out<<key<<" = "<<value<<'\n'; };
    out<<"CCSDS_OPM_VERS = 3.0\n";
    field("OBJECT_NAME",record.object_name); field("OBJECT_ID",record.object_id); field("CENTER_NAME",record.center_name);
    field("REF_FRAME",record.ref_frame); field("TIME_SYSTEM",record.time_system); field("EPOCH",record.epoch);
    out<<"X = "<<record.state_m.position_m.x/1000.0<<" [km]\nY = "<<record.state_m.position_m.y/1000.0<<" [km]\nZ = "<<record.state_m.position_m.z/1000.0<<" [km]\n";
    out<<"X_DOT = "<<record.state_m.velocity_m_s.x/1000.0<<" [km/s]\nY_DOT = "<<record.state_m.velocity_m_s.y/1000.0<<" [km/s]\nZ_DOT = "<<record.state_m.velocity_m_s.z/1000.0<<" [km/s]\n";
    return out.str();
}

} // namespace nadir::astro
