#pragma once
#include <cstdint>
#include <nadir/data/cache.hpp>
#include <nadir/data/http.hpp>
#include <nadir/data/source.hpp>
#include <string>

namespace nadir::data {

struct SyncResult {
    bool ok{};
    bool skipped{};
    std::string source_id;
    long status{};
    std::uint64_t bytes{};
    std::string sha256;
    std::string path;
    std::string error;
};

class SyncEngine {
public:
    explicit SyncEngine(std::string cache_root);
    SyncResult fetch(const Source& source) const;
private:
    HttpClient http_;
    CacheStore cache_;
};

}
