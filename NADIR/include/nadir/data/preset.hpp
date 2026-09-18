#pragma once
#include <optional>
#include <string>
#include <vector>

namespace nadir::data {

struct SyncPreset {
    std::string id;
    std::vector<std::string> source_ids;
    std::string description;
};

class PresetCatalog {
public:
    bool load(const std::string& path);
    const std::vector<SyncPreset>& all() const;
    std::optional<SyncPreset> find(const std::string& id) const;
private:
    std::vector<SyncPreset> presets_;
};

}
