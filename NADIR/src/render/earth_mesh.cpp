#include <nadir/render/earth_mesh.hpp>
#include <cmath>
#include <random>
#include <algorithm>

namespace nadir::render {

namespace {
constexpr double pi = 3.141592653589793238462643383279502884;
constexpr double two_pi = 2.0 * pi;
constexpr double wgs84_a_m = 6378137.0;
constexpr double wgs84_f = 1.0 / 298.257223563;
constexpr double wgs84_e2 = wgs84_f * (2.0 - wgs84_f);

nadir::math::Vec3d geodetic_surface_point(double latitude_rad, double longitude_rad) {
    const double sin_lat = std::sin(latitude_rad);
    const double cos_lat = std::cos(latitude_rad);
    const double n = wgs84_a_m / std::sqrt(1.0 - wgs84_e2 * sin_lat * sin_lat);
    return {
        n * cos_lat * std::cos(longitude_rad),
        n * cos_lat * std::sin(longitude_rad),
        n * (1.0 - wgs84_e2) * sin_lat
    };
}
}

EarthMesh generate_earth_mesh(int latitude_steps, int longitude_steps) {
    EarthMesh mesh;
    mesh.vertices.reserve((latitude_steps + 1) * (longitude_steps + 1));
    mesh.lines.reserve(latitude_steps * longitude_steps * 2);

    if (latitude_steps < 1 || longitude_steps < 3) return mesh;

    // Generate WGS84 geodetic surface points in Earth-centered SI metres.
    for (int lat_idx = 0; lat_idx <= latitude_steps; ++lat_idx) {
        const double lat = -pi/2 + pi * lat_idx / latitude_steps;
        for (int lon_idx = 0; lon_idx <= longitude_steps; ++lon_idx) {
            const double lon = two_pi * lon_idx / longitude_steps;
            mesh.vertices.push_back(geodetic_surface_point(lat, lon));
        }
    }

    // Generate latitude lines
    for (int lat_idx = 0; lat_idx <= latitude_steps; ++lat_idx) {
        for (int lon_idx = 0; lon_idx < longitude_steps; ++lon_idx) {
            const std::uint32_t a = lat_idx * (longitude_steps + 1) + lon_idx;
            const std::uint32_t b = lat_idx * (longitude_steps + 1) + (lon_idx + 1);
            mesh.lines.push_back(std::array<std::uint32_t,2>{a, b});
        }
    }

    // Generate longitude lines
    for (int lon_idx = 0; lon_idx <= longitude_steps; ++lon_idx) {
        for (int lat_idx = 0; lat_idx < latitude_steps; ++lat_idx) {
            const std::uint32_t a = lat_idx * (longitude_steps + 1) + lon_idx;
            const std::uint32_t b = (lat_idx + 1) * (longitude_steps + 1) + lon_idx;
            mesh.lines.push_back(std::array<std::uint32_t,2>{a, b});
        }
    }

    return mesh;
}

StarField generate_star_field(int count) {
    StarField field;
    field.stars.reserve(count);

    std::mt19937_64 rng(0xDEADBEEF);
    std::uniform_real_distribution<double> dist_ra(0.0, 2.0 * 3.14159265358979323846);
    std::uniform_real_distribution<double> dist_dec(-1.0, 1.0);
    std::uniform_real_distribution<float> dist_mag(0.0f, 6.0f);

    for (int i = 0; i < count; ++i) {
        const double ra = dist_ra(rng);
        const double dec = std::asin(dist_dec(rng));
        const double cos_dec = std::cos(dec);

        StarField::Star star;
        star.direction = {
            cos_dec * std::cos(ra),
            cos_dec * std::sin(ra),
            std::sin(dec)
        };
        star.magnitude = dist_mag(rng);
        field.stars.push_back(star);
    }

    // Sort by magnitude (brighter first)
    std::sort(field.stars.begin(), field.stars.end(),
              [](const StarField::Star& a, const StarField::Star& b) {
                  return a.magnitude < b.magnitude;
              });

    return field;
}

}
