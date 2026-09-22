#include <nadir/astro/catalog.hpp>

#include <algorithm>
#include <cctype>
#include <map>

namespace nadir::astro {
namespace {
std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}
void add_alias(std::vector<std::string>& aliases, const std::string& alias) {
    if (alias.empty() || std::find(aliases.begin(), aliases.end(), alias) != aliases.end()) return;
    aliases.push_back(alias);
}
}

std::vector<CatalogObject> normalize_catalog(const std::vector<OmmRecord>& records) {
    std::map<std::uint64_t, CatalogObject> indexed;
    for (const auto& record : records) {
        if (record.norad_cat_id == 0) continue;
        auto [it, inserted] = indexed.try_emplace(record.norad_cat_id, CatalogObject{record, {}});
        auto& object = it->second;
        if (!inserted && record.epoch > object.omm.epoch) object.omm = record;
        add_alias(object.aliases, record.object_name);
        add_alias(object.aliases, record.object_id);
    }
    std::vector<CatalogObject> result;
    result.reserve(indexed.size());
    for (auto& [_, object] : indexed) result.push_back(std::move(object));
    return result;
}

std::vector<CatalogObject> find_catalog(const std::vector<CatalogObject>& catalog, const std::string& query, std::size_t limit) {
    const auto needle = lower(query);
    std::vector<CatalogObject> result;
    for (const auto& object : catalog) {
        bool match = needle.empty() || std::to_string(object.omm.norad_cat_id) == needle || lower(object.omm.object_id).find(needle) != std::string::npos;
        for (const auto& alias : object.aliases) match = match || lower(alias).find(needle) != std::string::npos;
        if (!match) continue;
        result.push_back(object);
        if (result.size() == limit) break;
    }
    return result;
}

void attach_satcat_metadata(std::vector<CatalogObject>& catalog, const std::vector<SatcatRecord>& records) {
    std::map<std::uint64_t, const SatcatRecord*> indexed;
    for (const auto& record : records) if (record.norad_cat_id != 0) indexed[record.norad_cat_id] = &record;
    for (auto& object : catalog) {
        const auto found = indexed.find(object.omm.norad_cat_id);
        if (found == indexed.end()) continue;
        const auto& record = *found->second;
        object.object_type = record.object_type;
        object.ops_status_code = record.ops_status_code;
        object.owner = record.owner;
        object.launch_date = record.launch_date;
        object.decay_date = record.decay_date;
        object.data_status_code = record.data_status_code;
        object.orbit_center = record.orbit_center;
        object.orbit_type = record.orbit_type;
        object.active = record.decay_date.empty();
        add_alias(object.aliases, record.object_name);
        add_alias(object.aliases, record.object_id);
    }
}

} // namespace nadir::astro
