#include <nadir/astro/oem.hpp>

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

std::optional<double> number(const std::string& text) {
    try {
        std::size_t consumed{};
        const double value = std::stod(text, &consumed);
        return consumed == text.size() && std::isfinite(value) ? std::optional{value} : std::nullopt;
    } catch (...) { return std::nullopt; }
}

} // namespace

OemParseResult parse_oem_kvn(const std::string& text) {
    if (text.size() > maximum_message_bytes) return {false, {}, "OEM message exceeds size limit"};
    std::map<std::string, std::string> metadata;
    std::vector<OemStateSample> samples;
    bool in_metadata{};
    bool in_data{};
    std::size_t line_count{};
    std::istringstream input(text);
    for (std::string line; std::getline(input, line); ) {
        if (++line_count > maximum_lines) return {false, {}, "OEM message has too many lines"};
        line = trim(std::move(line));
        if (line.empty() || line.starts_with("COMMENT")) continue;
        if (line == "META_START") { if (in_metadata || in_data) return {false, {}, "invalid OEM metadata section"}; in_metadata = true; continue; }
        if (line == "META_STOP") { if (!in_metadata) return {false, {}, "invalid OEM metadata stop"}; in_metadata = false; continue; }
        if (line == "DATA_START") { if (in_metadata || in_data) return {false, {}, "invalid OEM data section"}; in_data = true; continue; }
        if (line == "DATA_STOP") { if (!in_data) return {false, {}, "invalid OEM data stop"}; in_data = false; continue; }
        if (in_data) {
            std::istringstream values(line);
            std::string epoch, x, y, z, vx, vy, vz, extra;
            if (!(values >> epoch >> x >> y >> z >> vx >> vy >> vz) || (values >> extra)) return {false, {}, "invalid OEM state row"};
            const auto px = number(x), py = number(y), pz = number(z), pvx = number(vx), pvy = number(vy), pvz = number(vz);
            if (!px || !py || !pz || !pvx || !pvy || !pvz) return {false, {}, "invalid OEM state value"};
            samples.push_back({epoch, {{*px * 1000.0, *py * 1000.0, *pz * 1000.0}, {*pvx * 1000.0, *pvy * 1000.0, *pvz * 1000.0}}});
            continue;
        }
        const auto equals = line.find('=');
        if (equals == std::string::npos) continue;
        auto key = trim(line.substr(0, equals));
        key.erase(std::remove_if(key.begin(), key.end(), [](unsigned char c) { return std::isspace(c); }), key.end());
        metadata[key] = trim(line.substr(equals + 1));
    }
    const auto required = [&](const char* key) { return metadata.contains(key) && !metadata[key].empty(); };
    if (in_metadata || in_data || !required("CCSDS_OEM_VERS") || !required("OBJECT_NAME") || !required("OBJECT_ID") ||
        !required("CENTER_NAME") || !required("REF_FRAME") || !required("TIME_SYSTEM") || samples.empty())
        return {false, {}, "missing OEM metadata or data"};
    return {true, OemRecord{metadata["OBJECT_NAME"], metadata["OBJECT_ID"], metadata["CENTER_NAME"],
                            metadata["REF_FRAME"], metadata["TIME_SYSTEM"], std::move(samples)}, {}};
}

std::string write_oem_kvn(const OemRecord& record) {
    if (record.samples.empty()) return {};
    std::ostringstream out;
    out.precision(17);
    out << "CCSDS_OEM_VERS = 2.0\nMETA_START\n"
        << "OBJECT_NAME = " << record.object_name << "\nOBJECT_ID = " << record.object_id << "\nCENTER_NAME = " << record.center_name
        << "\nREF_FRAME = " << record.ref_frame << "\nTIME_SYSTEM = " << record.time_system << "\nMETA_STOP\nDATA_START\n";
    for (const auto& sample : record.samples)
        out << sample.epoch << ' ' << sample.state_m.position_m.x / 1000.0 << ' ' << sample.state_m.position_m.y / 1000.0 << ' '
            << sample.state_m.position_m.z / 1000.0 << ' ' << sample.state_m.velocity_m_s.x / 1000.0 << ' '
            << sample.state_m.velocity_m_s.y / 1000.0 << ' ' << sample.state_m.velocity_m_s.z / 1000.0 << '\n';
    out << "DATA_STOP\n";
    return out.str();
}

} // namespace nadir::astro
