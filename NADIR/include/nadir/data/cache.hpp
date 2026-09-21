#pragma once
#include <cstdint>
#include <nadir/data/source.hpp>
#include <optional>
#include <string>

namespace nadir::data {

struct CachedObject {
    std::string data_path;
    std::string meta_path;
    std::string sha256;
    std::uint64_t bytes{};
};

struct CacheInfo {
    std::string source_id;
    std::string data_path;
    std::string meta_path;
    std::string sha256;
    std::string url;
    std::string format;
    std::string license;
    std::int64_t fetched_unix_ns{};
    std::uint64_t bytes{};
    long http_status{};
};

class CacheStore {
public:
    explicit CacheStore(std::string root);
    std::optional<CachedObject> store(const Source& source,const std::string& body,long http_status) const;
    std::optional<std::string> latest(const std::string& source_id) const;
    std::optional<std::int64_t> last_fetch_ns(const std::string& source_id) const;
    std::optional<CacheInfo> info(const std::string& source_id) const;
    // Verifies that the cached bytes still match the manifest digest and size.
    bool verify(const std::string& source_id) const;
private:
    std::string root_;
};

}
