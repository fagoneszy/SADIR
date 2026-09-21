#pragma once

#include <string>
#include <vector>

#include <nadir/astro/omm.hpp>

namespace nadir::astro {

struct CatalogObject {
    OmmRecord omm;
    std::vector<std::string> aliases;
    bool active{true};
};

// Builds a deterministic catalog keyed by 64-bit NORAD ID. Duplicate records
// retain the newest epoch and all distinct object-name/COSPAR aliases.
std::vector<CatalogObject> normalize_catalog(const std::vector<OmmRecord>& records);
std::vector<CatalogObject> find_catalog(const std::vector<CatalogObject>& catalog,
                                        const std::string& query, std::size_t limit = 50);

} // namespace nadir::astro
