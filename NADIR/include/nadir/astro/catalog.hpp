#pragma once

#include <string>
#include <vector>

#include <nadir/astro/omm.hpp>
#include <nadir/astro/satcat.hpp>

namespace nadir::astro {

struct CatalogObject {
    OmmRecord omm;
    std::vector<std::string> aliases;
    bool active{true};
    std::string object_type;
    std::string ops_status_code;
    std::string owner;
    std::string launch_date;
    std::string decay_date;
    std::string data_status_code;
    std::string orbit_center;
    std::string orbit_type;
};

// Builds a deterministic catalog keyed by 64-bit NORAD ID. Duplicate records
// retain the newest epoch and all distinct object-name/COSPAR aliases.
std::vector<CatalogObject> normalize_catalog(const std::vector<OmmRecord>& records);
std::vector<CatalogObject> find_catalog(const std::vector<CatalogObject>& catalog,
                                        const std::string& query, std::size_t limit = 50);
void attach_satcat_metadata(std::vector<CatalogObject>& catalog, const std::vector<SatcatRecord>& records);

} // namespace nadir::astro
