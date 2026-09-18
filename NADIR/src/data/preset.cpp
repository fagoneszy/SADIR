#include <nadir/data/preset.hpp>
#include <fstream>
#include <sstream>

namespace nadir::data {

static std::vector<std::string> split(const std::string& s,char d) {
    std::vector<std::string> out;
    std::string item;
    std::stringstream ss(s);
    while (std::getline(ss,item,d)) if (!item.empty()) out.push_back(item);
    return out;
}

bool PresetCatalog::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) return false;
    presets_.clear();
    std::string line;
    if (!std::getline(in,line)) return false;
    while (std::getline(in,line)) {
        if (line.empty()) continue;
        const auto f=split(line,'\t');
        if (f.size()<3) continue;
        presets_.push_back({f[0],split(f[1],';'),f[2]});
    }
    return !presets_.empty();
}

const std::vector<SyncPreset>& PresetCatalog::all() const { return presets_; }

std::optional<SyncPreset> PresetCatalog::find(const std::string& id) const {
    for (const auto& p:presets_) if (p.id==id) return p;
    return std::nullopt;
}

}
