#include <nadir/astro/catalog.hpp>

int main() {
    nadir::astro::OmmRecord old{}; old.norad_cat_id = 25544; old.object_name = "ISS"; old.object_id = "1998-067A"; old.epoch = "2026-01-01T00:00:00";
    auto current = old; current.object_name = "ISS (ZARYA)"; current.epoch = "2026-02-01T00:00:00";
    nadir::astro::OmmRecord large{}; large.norad_cat_id = 5'000'000'000ULL; large.object_name = "BIG-ID"; large.epoch = current.epoch;
    const auto catalog = nadir::astro::normalize_catalog({old, current, large});
    if (catalog.size() != 2 || catalog.front().omm.epoch != current.epoch || catalog.front().aliases.size() != 3) return 1;
    if (nadir::astro::find_catalog(catalog, "1998-067a").size() != 1) return 2;
    return nadir::astro::find_catalog(catalog, "5000000000").size() == 1 ? 0 : 3;
}
