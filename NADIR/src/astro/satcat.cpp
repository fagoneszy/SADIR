#include <nadir/astro/satcat.hpp>
#include <nadir/core/json.hpp>

#include <cmath>

namespace nadir::astro {
namespace {
constexpr std::size_t maximum_satcat_bytes = 16U * 1024U * 1024U;
std::string string(const json::Value& value, const char* key) {
    const auto* field = value.get(key);
    return field ? field->as_string() : std::string{};
}
std::uint64_t identifier(const json::Value& value) {
    const auto* field = value.get("NORAD_CAT_ID");
    if (!field) return 0;
    try {
        if (field->is_string()) return std::stoull(field->as_string());
        const auto number = field->as_number();
        return std::isfinite(number) && number > 0.0 && std::floor(number) == number
            ? static_cast<std::uint64_t>(number) : 0;
    } catch (...) { return 0; }
}
std::optional<SatcatRecord> record(const json::Value& value) {
    if (!value.is_object()) return std::nullopt;
    SatcatRecord result;
    result.norad_cat_id = identifier(value);
    result.object_name = string(value, "OBJECT_NAME");
    result.object_id = string(value, "OBJECT_ID");
    result.object_type = string(value, "OBJECT_TYPE");
    result.ops_status_code = string(value, "OPS_STATUS_CODE");
    result.owner = string(value, "OWNER");
    result.launch_date = string(value, "LAUNCH_DATE");
    result.decay_date = string(value, "DECAY_DATE");
    result.data_status_code = string(value, "DATA_STATUS_CODE");
    result.orbit_center = string(value, "ORBIT_CENTER");
    result.orbit_type = string(value, "ORBIT_TYPE");
    return result.norad_cat_id == 0 ? std::nullopt : std::optional<SatcatRecord>{std::move(result)};
}
}

SatcatParseResult parse_satcat_json(const std::string& text) {
    if (text.size() > maximum_satcat_bytes) return {false, {}, "SATCAT message exceeds size limit"};
    const auto parsed = json::parse(text);
    if (!parsed.ok) return {false, {}, parsed.error + " at " + std::to_string(parsed.offset)};
    std::vector<SatcatRecord> records;
    const auto append = [&](const json::Value& value) { if (const auto parsed_record = record(value)) records.push_back(*parsed_record); };
    if (const auto* array = parsed.value.as_array()) for (const auto& value : *array) append(value);
    else append(parsed.value);
    return records.empty() ? SatcatParseResult{false, {}, "no SATCAT records"} : SatcatParseResult{true, std::move(records), {}};
}
} // namespace nadir::astro
