#pragma once
#include <cstdint>
#include <optional>
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

// Returns an explanation when a fetched body violates the declared source
// format. This check runs before cache persistence.
std::optional<std::string> content_validation_error(const Source& source, const std::string& body);

class SyncEngine {
public:
    explicit SyncEngine(std::string cache_root);
    SyncResult fetch(const Source& source) const;
private:
    HttpClient http_;
    CacheStore cache_;
};

}
