#include <nadir/data/cache.hpp>
#include <nadir/core/sha256.hpp>
#include <nadir/core/time.hpp>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <unordered_map>

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
    const auto base=core::compact_utc(now.unix_ns)+"-"+std::to_string(now.unix_ns % 1000000000LL);
    const auto data_path=dir/(base+"."+ext);
    const auto meta_path=dir/(base+".meta");
    const auto data_partial=data_path.string()+".partial";
    const auto meta_partial=meta_path.string()+".partial";
    std::ofstream data(data_partial,std::ios::binary|std::ios::trunc);
    if (!data) return std::nullopt;
    data.write(body.data(),static_cast<std::streamsize>(body.size()));
    data.close();
    std::error_code error;
    std::filesystem::rename(data_partial,data_path,error);
    if (error) { std::filesystem::remove(data_partial); return std::nullopt; }
    const auto digest=core::sha256(body);
    std::ofstream meta(meta_partial,std::ios::trunc);
    if (!meta) return std::nullopt;
    meta<<"source="<<source.id<<"\n";
    meta<<"domain="<<source.domain<<"\n";
    meta<<"format="<<source.format<<"\n";
    meta<<"authority="<<source.authority<<"\n";
    meta<<"coverage="<<source.coverage<<"\n";
    meta<<"license="<<source.license<<"\n";
    meta<<"fetched="<<core::iso8601_utc(now.unix_ns)<<"\n";
    meta<<"fetched_unix_ns="<<now.unix_ns<<"\n";
    meta<<"http_status="<<http_status<<"\n";
    meta<<"bytes="<<body.size()<<"\n";
    meta<<"sha256="<<digest<<"\n";
    meta<<"url="<<source.url<<"\n";
    meta.close();
    std::filesystem::rename(meta_partial,meta_path,error);
    if (error) { std::filesystem::remove(meta_partial); return std::nullopt; }
    const auto latest_partial=(dir/"LATEST.partial");
    std::ofstream latest(latest_partial,std::ios::trunc);
    if (!latest) return std::nullopt;
    latest<<data_path.filename().string();
    latest.close();
    std::filesystem::rename(latest_partial,dir/"LATEST",error);
    if (error) {
        std::filesystem::remove(dir/"LATEST",error);
        error.clear();
        std::filesystem::rename(latest_partial,dir/"LATEST",error);
        if (error) { std::filesystem::remove(latest_partial); return std::nullopt; }
    }
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

std::optional<CacheInfo> CacheStore::info(const std::string& source_id) const {
    const auto data_path=latest(source_id);
    if (!data_path) return std::nullopt;
    auto meta_path=std::filesystem::path(*data_path);
    meta_path.replace_extension(".meta");
    std::ifstream in(meta_path);
    if (!in) return std::nullopt;
    std::unordered_map<std::string,std::string> kv;
    std::string line;
    while (std::getline(in,line)) {
        const auto p=line.find('=');
        if (p!=std::string::npos) kv[line.substr(0,p)]=line.substr(p+1);
    }
    CacheInfo out;
    out.source_id=source_id;
    out.data_path=*data_path;
    out.meta_path=meta_path.string();
    out.sha256=kv["sha256"];
    out.url=kv["url"];
    out.format=kv["format"];
    out.license=kv["license"];
    try { out.fetched_unix_ns=std::stoll(kv["fetched_unix_ns"]); } catch (...) {}
    try { out.bytes=static_cast<std::uint64_t>(std::stoull(kv["bytes"])); } catch (...) {}
    try { out.http_status=std::stol(kv["http_status"]); } catch (...) {}
    return out;
}

bool CacheStore::verify(const std::string& source_id) const {
    const auto cached=info(source_id);
    if (!cached || cached->sha256.size()!=64 || !std::filesystem::exists(cached->data_path)) return false;
    std::error_code error;
    const auto bytes=std::filesystem::file_size(cached->data_path,error);
    return !error && bytes==cached->bytes && core::sha256_file(cached->data_path)==cached->sha256;
}

std::optional<std::int64_t> CacheStore::last_fetch_ns(const std::string& source_id) const {
    const auto i=info(source_id);
    if (!i) return std::nullopt;
    return i->fetched_unix_ns;
}

}
