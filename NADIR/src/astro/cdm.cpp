#include <nadir/astro/cdm.hpp>

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
    const auto found = fields.find(key);
    if (found == fields.end()) return std::nullopt;
    try {
        std::istringstream input(found->second);
        double value{};
        std::string trailing;
        if (!(input >> value) || !std::isfinite(value) || (input >> trailing && !trailing.starts_with("["))) return std::nullopt;
        return value;
    } catch (...) { return std::nullopt; }
}
} // namespace

CdmParseResult parse_cdm_kvn(const std::string& text) {
    if (text.size() > maximum_message_bytes) return {false, {}, "CDM message exceeds size limit"};
    std::map<std::string, std::string> fields;
    std::istringstream input(text);
    std::size_t line_count{};
    for (std::string line; std::getline(input, line); ) {
        if (++line_count > maximum_lines) return {false, {}, "CDM message has too many lines"};
        const auto equals = line.find('=');
        if (equals == std::string::npos) continue;
        auto key = trim(line.substr(0, equals));
        key.erase(std::remove_if(key.begin(), key.end(), [](unsigned char c) { return std::isspace(c); }), key.end());
        fields[key] = trim(line.substr(equals + 1));
    }
    const auto required = [&](const char* key) { return fields.contains(key) && !fields[key].empty(); };
    if (!required("CCSDS_CDM_VERS") || !required("TCA") || !required("OBJECT1_OBJECT_DESIGNATOR") || !required("OBJECT2_OBJECT_DESIGNATOR"))
        return {false, {}, "missing CDM metadata"};
    const auto miss = number(fields, "MISS_DISTANCE");
    const auto speed = number(fields, "RELATIVE_SPEED");
    if (!miss || !speed || *miss < 0.0 || *speed < 0.0) return {false, {}, "missing CDM conjunction data"};
    std::optional<double> probability;
    if (fields.contains("COLLISION_PROBABILITY")) {
        probability = number(fields, "COLLISION_PROBABILITY");
        if (!probability || *probability < 0.0 || *probability > 1.0) return {false, {}, "invalid CDM collision probability"};
    }
    return {true, CdmRecord{fields["TCA"], fields["OBJECT1_OBJECT_DESIGNATOR"], fields["OBJECT2_OBJECT_DESIGNATOR"],
                            *miss * 1000.0, *speed * 1000.0, probability}, {}};
}

} // namespace nadir::astro
