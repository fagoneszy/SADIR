#include <nadir/data/source.hpp>
#include <fstream>
#include <sstream>

namespace nadir::data {

static std::vector<std::string> split_tab(const std::string& line) {
    std::vector<std::string> out;
    std::string item;
    std::stringstream ss(line);
    while (std::getline(ss,item,'\t')) out.push_back(item);
    return out;
}

bool SourceCatalog::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) return false;
    sources_.clear();
    std::string line;
    while (std::getline(in,line)) {
        if (line.empty()) continue;
        auto f=split_tab(line);
        if (f.size()<8 || f[0]=="id") continue;
        Source s;
        s.id=f[0];
        s.domain=f[1];
        s.format=f[2];
        s.interval_seconds=static_cast<std::uint64_t>(std::stoull(f[3]));
        s.auth=f[4];
        s.syncable=f[5]=="1";
        s.url=f[6];
        s.description=f[7];
        sources_.push_back(std::move(s));
    }
    return true;
}

const std::vector<Source>& SourceCatalog::all() const { return sources_; }

std::optional<Source> SourceCatalog::find(const std::string& id) const {
    for (const auto& s:sources_) if (s.id==id) return s;
    return std::nullopt;
}

std::vector<Source> SourceCatalog::domain(const std::string& name) const {
    std::vector<Source> out;
    for (const auto& s:sources_) if (s.domain==name) out.push_back(s);
    return out;
}

}
