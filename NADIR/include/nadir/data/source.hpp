#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace nadir::data {

struct Source {
    std::string id;
    std::string domain;
    std::string format;
    std::uint64_t interval_seconds{};
    std::string auth;
    bool syncable{};
    std::string authority;
    std::string coverage;
    std::string license;
    int priority{};
    std::string url;
    std::string description;
};

class SourceCatalog {
public:
    bool load(const std::string& path);
    const std::vector<Source>& all() const;
    std::optional<Source> find(const std::string& id) const;
    std::vector<Source> domain(const std::string& name) const;
    std::vector<Source> search(const std::string& query) const;
private:
    std::vector<Source> sources_;
};

}
