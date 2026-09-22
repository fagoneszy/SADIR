#include <nadir/astro/catalog.hpp>

int main() {
    nadir::astro::OmmRecord old{}; old.norad_cat_id = 25544; old.object_name = "ISS"; old.object_id = "1998-067A"; old.epoch = "2026-01-01T00:00:00";
    auto current = old; current.object_name = "ISS (ZARYA)"; current.epoch = "2026-02-01T00:00:00";
    nadir::astro::OmmRecord large{}; large.norad_cat_id = 5'000'000'000ULL; large.object_name = "BIG-ID"; large.epoch = current.epoch;
    const auto catalog = nadir::astro::normalize_catalog({old, current, large});
    if (catalog.size() != 2 || catalog.front().omm.epoch != current.epoch || catalog.front().aliases.size() != 3) return 1;
    if (nadir::astro::find_catalog(catalog, "1998-067a").size() != 1) return 2;
    const auto satcat = nadir::astro::parse_satcat_json("[{\"NORAD_CAT_ID\":25544,\"OBJECT_NAME\":\"ISS\",\"OBJECT_TYPE\":\"PAY\",\"OPS_STATUS_CODE\":\"+\",\"OWNER\":\"US\",\"LAUNCH_DATE\":\"1998-11-20\",\"ORBIT_CENTER\":\"EA\",\"ORBIT_TYPE\":\"ORB\"},{\"NORAD_CAT_ID\":5000000000,\"OBJECT_TYPE\":\"DEB\",\"DECAY_DATE\":\"2026-02-02\",\"DATA_STATUS_CODE\":\"NCE\"}]");
    if (!satcat.ok || satcat.records.size() != 2) return 3;
    auto enriched = catalog;
    nadir::astro::attach_satcat_metadata(enriched, satcat.records);
    if (enriched[0].object_type != "PAY" || enriched[0].ops_status_code != "+" || enriched[0].owner != "US" || !enriched[0].active ||
        enriched[1].object_type != "DEB" || enriched[1].active || enriched[1].data_status_code != "NCE") return 4;
    if (nadir::astro::parse_satcat_json("{\"NORAD_CAT_ID\":0}").ok) return 5;
    return nadir::astro::find_catalog(enriched, "5000000000").size() == 1 ? 0 : 6;
}
