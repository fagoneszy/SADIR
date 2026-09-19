#include <nadir/space/solar_catalog.hpp>
#include <array>

namespace nadir::space {

namespace {

constexpr BodyParams sun = {
    .name = "Sun",
    .horizons_id = 10,
    .physical = {
        .equatorial_radius_km = 695700.0,
        .mean_radius_km = 695700.0,
        .mass_kg = 1.9885e30,
        .sidereal_rotation_period_hours = 609.12,
        .obliquity_deg = 7.25,
        .retrograde = false
    },
    .orbital = {
        .sidereal_orbital_period_days = 0.0,
        .orbital_inclination_deg = 0.0,
        .orbital_eccentricity = 0.0,
        .mean_distance_from_sun_km = 0.0,
        .perihelion_km = 0.0,
        .aphelion_km = 0.0,
        .orbital_velocity_km_s = 0.0
    }
};

constexpr BodyParams mercury = {
    .name = "Mercury",
    .horizons_id = 199,
    .physical = {
        .equatorial_radius_km = 2439.7,
        .mean_radius_km = 2439.7,
        .mass_kg = 3.3011e23,
        .sidereal_rotation_period_hours = 1407.6,
        .obliquity_deg = 0.034,
        .retrograde = false
    },
    .orbital = {
        .sidereal_orbital_period_days = 87.969,
        .orbital_inclination_deg = 7.005,
        .orbital_eccentricity = 0.20563,
        .mean_distance_from_sun_km = 57909227.0,
        .perihelion_km = 46001200.0,
        .aphelion_km = 69816900.0,
        .orbital_velocity_km_s = 47.36
    }
};

constexpr BodyParams venus = {
    .name = "Venus",
    .horizons_id = 299,
    .physical = {
        .equatorial_radius_km = 6051.8,
        .mean_radius_km = 6051.8,
        .mass_kg = 4.8675e24,
        .sidereal_rotation_period_hours = -5832.5,
        .obliquity_deg = 177.36,
        .retrograde = true
    },
    .orbital = {
        .sidereal_orbital_period_days = 224.701,
        .orbital_inclination_deg = 3.39458,
        .orbital_eccentricity = 0.006772,
        .mean_distance_from_sun_km = 108209475.0,
        .perihelion_km = 107477000.0,
        .aphelion_km = 108941000.0,
        .orbital_velocity_km_s = 35.02
    }
};

constexpr BodyParams earth = {
    .name = "Earth",
    .horizons_id = 399,
    .physical = {
        .equatorial_radius_km = 6378.137,
        .mean_radius_km = 6371.008,
        .mass_kg = 5.97237e24,
        .sidereal_rotation_period_hours = 23.9345,
        .obliquity_deg = 23.4393,
        .retrograde = false
    },
    .orbital = {
        .sidereal_orbital_period_days = 365.256,
        .orbital_inclination_deg = 0.0,
        .orbital_eccentricity = 0.0167086,
        .mean_distance_from_sun_km = 149598023.0,
        .perihelion_km = 147095000.0,
        .aphelion_km = 152100000.0,
        .orbital_velocity_km_s = 29.78
    }
};

constexpr BodyParams moon = {
    .name = "Moon",
    .horizons_id = 301,
    .physical = {
        .equatorial_radius_km = 1738.1,
        .mean_radius_km = 1737.4,
        .mass_kg = 7.342e22,
        .sidereal_rotation_period_hours = 655.728,
        .obliquity_deg = 6.687,
        .retrograde = false
    },
    .orbital = {
        .sidereal_orbital_period_days = 27.321661,
        .orbital_inclination_deg = 5.145,
        .orbital_eccentricity = 0.0549,
        .mean_distance_from_sun_km = 149598023.0,
        .perihelion_km = 147095000.0,
        .aphelion_km = 152100000.0,
        .orbital_velocity_km_s = 1.022
    }
};

constexpr BodyParams mars = {
    .name = "Mars",
    .horizons_id = 499,
    .physical = {
        .equatorial_radius_km = 3396.2,
        .mean_radius_km = 3389.5,
        .mass_kg = 6.4171e23,
        .sidereal_rotation_period_hours = 24.6229,
        .obliquity_deg = 25.19,
        .retrograde = false
    },
    .orbital = {
        .sidereal_orbital_period_days = 686.980,
        .orbital_inclination_deg = 1.850,
        .orbital_eccentricity = 0.09341,
        .mean_distance_from_sun_km = 227943824.0,
        .perihelion_km = 206655215.0,
        .aphelion_km = 249232432.0,
        .orbital_velocity_km_s = 24.07
    }
};

constexpr BodyParams jupiter = {
    .name = "Jupiter",
    .horizons_id = 599,
    .physical = {
        .equatorial_radius_km = 71492.0,
        .mean_radius_km = 69911.0,
        .mass_kg = 1.8982e27,
        .sidereal_rotation_period_hours = 9.925,
        .obliquity_deg = 3.13,
        .retrograde = false
    },
    .orbital = {
        .sidereal_orbital_period_days = 4332.59,
        .orbital_inclination_deg = 1.305,
        .orbital_eccentricity = 0.0489,
        .mean_distance_from_sun_km = 778570000.0,
        .perihelion_km = 740520000.0,
        .aphelion_km = 816620000.0,
        .orbital_velocity_km_s = 13.07
    }
};

constexpr BodyParams saturn = {
    .name = "Saturn",
    .horizons_id = 699,
    .physical = {
        .equatorial_radius_km = 60268.0,
        .mean_radius_km = 58232.0,
        .mass_kg = 5.6834e26,
        .sidereal_rotation_period_hours = 10.656,
        .obliquity_deg = 26.73,
        .retrograde = false
    },
    .orbital = {
        .sidereal_orbital_period_days = 10759.22,
        .orbital_inclination_deg = 2.485,
        .orbital_eccentricity = 0.0565,
        .mean_distance_from_sun_km = 1433500000.0,
        .perihelion_km = 1352550000.0,
        .aphelion_km = 1514500000.0,
        .orbital_velocity_km_s = 9.68
    }
};

constexpr BodyParams uranus = {
    .name = "Uranus",
    .horizons_id = 799,
    .physical = {
        .equatorial_radius_km = 25559.0,
        .mean_radius_km = 25362.0,
        .mass_kg = 8.6810e25,
        .sidereal_rotation_period_hours = -17.24,
        .obliquity_deg = 97.77,
        .retrograde = true
    },
    .orbital = {
        .sidereal_orbital_period_days = 30688.5,
        .orbital_inclination_deg = 0.77,
        .orbital_eccentricity = 0.0472,
        .mean_distance_from_sun_km = 2872500000.0,
        .perihelion_km = 2741300000.0,
        .aphelion_km = 3003620000.0,
        .orbital_velocity_km_s = 6.80
    }
};

constexpr BodyParams neptune = {
    .name = "Neptune",
    .horizons_id = 899,
    .physical = {
        .equatorial_radius_km = 24764.0,
        .mean_radius_km = 24622.0,
        .mass_kg = 1.0241e26,
        .sidereal_rotation_period_hours = 16.11,
        .obliquity_deg = 28.32,
        .retrograde = false
    },
    .orbital = {
        .sidereal_orbital_period_days = 60182.0,
        .orbital_inclination_deg = 1.77,
        .orbital_eccentricity = 0.0086,
        .mean_distance_from_sun_km = 4495100000.0,
        .perihelion_km = 4444450000.0,
        .aphelion_km = 4545700000.0,
        .orbital_velocity_km_s = 5.43
    }
};

constexpr BodyParams pluto = {
    .name = "Pluto",
    .horizons_id = 999,
    .physical = {
        .equatorial_radius_km = 1188.3,
        .mean_radius_km = 1188.3,
        .mass_kg = 1.303e22,
        .sidereal_rotation_period_hours = -153.29,
        .obliquity_deg = 122.5,
        .retrograde = true
    },
    .orbital = {
        .sidereal_orbital_period_days = 90560.0,
        .orbital_inclination_deg = 17.14,
        .orbital_eccentricity = 0.2488,
        .mean_distance_from_sun_km = 5906380000.0,
        .perihelion_km = 4436820000.0,
        .aphelion_km = 7375930000.0,
        .orbital_velocity_km_s = 4.74
    }
};

constexpr std::array<const BodyParams*, 10> catalog = {
    &sun, &mercury, &venus, &earth, &moon,
    &mars, &jupiter, &saturn, &uranus, &neptune
};

} // namespace

const BodyParams& solar_body(std::uint32_t index) {
    return *catalog.at(index);
}

const BodyParams* solar_body_by_name(const std::string& name) noexcept {
    for (const auto* body : catalog) {
        if (body->name == name) return body;
    }
    return nullptr;
}

const BodyParams* solar_body_by_horizons_id(int id) noexcept {
    for (const auto* body : catalog) {
        if (body->horizons_id == id) return body;
    }
    return nullptr;
}

std::uint32_t solar_body_count() noexcept {
    return static_cast<std::uint32_t>(catalog.size());
}

} // namespace nadir::space