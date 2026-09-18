#pragma once
#include <optional>
#include <string>
#include <vector>

namespace nadir::astro {

struct BodyInfo {
    std::string name;
    int horizons_id{};
    double equatorial_radius_km{};
    double mean_radius_km{};
    double mass_1e24kg{};
    double density_g_cm3{};
    double rotation_days{};
    double orbital_years{};
    double gravity_m_s2{};
    double escape_km_s{};
};

const std::vector<BodyInfo>& bodies();
std::optional<BodyInfo> find_body(const std::string& name);
std::string horizons_vectors_url(int target_id,const std::string& start,const std::string& stop);

}
