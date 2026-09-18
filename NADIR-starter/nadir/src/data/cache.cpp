#include <nadir/data/cache.hpp>
#include <nadir/core/sha256.hpp>
#include <nadir/core/time.hpp>
#include <algorithm>
#include <filesystem>
#include <fstream>

namespace nadir::data {

static std::string safe(std::string s) {
    for (auto& c:s) if (!std::isalnum(static_cast<unsigned char>(c)) && c!='-' && c!='_') c='_';
    return s;
}

CacheStore::CacheStore(std::string root):root_(std::move(root)) {}

std::optional<CachedObject> CacheStore::store(const Source& source,const std::string& body,long http_status) const {
    const auto now=core::now_utc();
    const auto dir=std::filesystem::path(root_)/safe(source.id);
    std::filesystem::create_directories(dir);
    const auto ext=source.format.empty()?"bin":source.format;
    const auto base=core::compact_utc(now.unix_ns);
    const auto data_path=dir/(base+"."+ext);
    const auto meta_path=dir/(base+".meta");
    std::ofstream data(data_path,std::ios::binary);
    if (!data) return std::nullopt;
    data.write(body.data(),static_cast<std::streamsize>(body.size()));
    data.close();
    const auto digest=core::sha256(body);
    std::ofstream meta(meta_path);
    if (!meta) return std::nullopt;
    meta<<"source="<<source.id<<"\n";
    meta<<"domain="<<source.domain<<"\n";
    meta<<"format="<<source.format<<"\n";
    meta<<"fetched="<<core::iso8601_utc(now.unix_ns)<<"\n";
    meta<<"fetched_unix_ns="<<now.unix_ns<<"\n";
    meta<<"http_status="<<http_status<<"\n";
    meta<<"bytes="<<body.size()<<"\n";
    meta<<"sha256="<<digest<<"\n";
    meta<<"url="<<source.url<<"\n";
    meta.close();
    std::ofstream latest(dir/"LATEST",std::ios::trunc);
    latest<<data_path.filename().string();
    return CachedObject{data_path.string(),meta_path.string(),digest,static_cast<std::uint64_t>(body.size())};
}

std::optional<std::string> CacheStore::latest(const std::string& source_id) const {
    const auto dir=std::filesystem::path(root_)/safe(source_id);
    std::ifstream in(dir/"LATEST");
    if (!in) return std::nullopt;
    std::string name;
    std::getline(in,name);
    if (name.empty()) return std::nullopt;
    return (dir/name).string();
}

std::optional<std::int64_t> CacheStore::last_fetch_ns(const std::string& source_id) const {
    const auto data=latest(source_id);
    if (!data) return std::nullopt;
    auto meta_path=std::filesystem::path(*data);
    meta_path.replace_extension(".meta");
    std::ifstream in(meta_path);
    if (!in) return std::nullopt;
    std::string line;
    while (std::getline(in,line)) {
        const std::string key="fetched_unix_ns=";
        if (line.rfind(key,0)==0) {
            try { return std::stoll(line.substr(key.size())); } catch (...) { return std::nullopt; }
        }
    }
    return std::nullopt;
}

}
