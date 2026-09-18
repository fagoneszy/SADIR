#include <nadir/data/source.hpp>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace nadir::data {

static std::vector<std::string> split_tab(const std::string& line) {
    std::vector<std::string> out;
    std::string item;
    std::stringstream ss(line);
    while (std::getline(ss,item,'\t')) out.push_back(item);
    if (!line.empty() && line.back()=='\t') out.emplace_back();
    return out;
}

static std::string lower(std::string s) {
    std::transform(s.begin(),s.end(),s.begin(),[](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return s;
}

bool SourceCatalog::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) return false;
    sources_.clear();
    std::string line;
    std::unordered_map<std::string,std::size_t> col;
    if (!std::getline(in,line)) return false;
    const auto header=split_tab(line);
    for (std::size_t i=0;i<header.size();++i) col[header[i]]=i;
    auto get=[&](const std::vector<std::string>& f,const std::string& k)->std::string {
        const auto it=col.find(k);
        return it==col.end() || it->second>=f.size()?std::string{}:f[it->second];
    };
    while (std::getline(in,line)) {
        if (line.empty()) continue;
        const auto f=split_tab(line);
        Source s;
        s.id=get(f,"id");
        s.domain=get(f,"domain");
        s.format=get(f,"format");
        try { s.interval_seconds=static_cast<std::uint64_t>(std::stoull(get(f,"interval_seconds"))); } catch (...) { s.interval_seconds=0; }
        s.auth=get(f,"auth");
        s.syncable=get(f,"syncable")=="1";
        s.authority=get(f,"authority");
        s.coverage=get(f,"coverage");
        s.license=get(f,"license");
        try { s.priority=std::stoi(get(f,"priority")); } catch (...) { s.priority=0; }
        s.url=get(f,"url");
        s.description=get(f,"description");
        if (!s.id.empty()) sources_.push_back(std::move(s));
    }
    return !sources_.empty();
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

std::vector<Source> SourceCatalog::search(const std::string& query) const {
    std::vector<Source> out;
    const auto q=lower(query);
    for (const auto& s:sources_) {
        const auto hay=lower(s.id+" "+s.domain+" "+s.authority+" "+s.coverage+" "+s.description);
        if (hay.find(q)!=std::string::npos) out.push_back(s);
    }
    return out;
}

}
