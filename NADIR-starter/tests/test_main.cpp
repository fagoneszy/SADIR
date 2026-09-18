#include <nadir/astro/horizons.hpp>
#include <nadir/core/sha256.hpp>
#include <nadir/geo/wgs84.hpp>
#include <cmath>
#include <iostream>

int main() {
    const nadir::geo::Geodetic g{0.0,0.0,0.0};
    const auto e=nadir::geo::geodetic_to_ecef(g);
    if (std::abs(e.x-nadir::geo::a)>0.001) return 1;
    if (std::abs(e.y)>0.001||std::abs(e.z)>0.001) return 2;
    const auto back=nadir::geo::ecef_to_geodetic(e);
    if (std::abs(back.latitude_deg)>1e-8||std::abs(back.longitude_deg)>1e-8) return 3;
    if (nadir::core::sha256("abc")!="ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") return 4;
    const std::string h="x\n$$SOE\n2460000.5, A.D. 2023-Jan-01 00:00:00.0000, 1, 2, 3, 4, 5, 6, 7, 8, 9,\n$$EOE\ny";
    const auto s=nadir::astro::parse_horizons_vector_csv(h);
    if (!s) return 5;
    if (s->range_km!=8.0||s->range_rate_km_s!=9.0) return 6;
    std::cout<<"OK\n";
    return 0;
}
