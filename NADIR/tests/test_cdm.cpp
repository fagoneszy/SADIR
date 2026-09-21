#include <nadir/astro/cdm.hpp>

#include <cmath>
#include <cstdint>
#include <string>

int main() {
    const auto parsed = nadir::astro::parse_cdm_kvn(R"(CCSDS_CDM_VERS = 1.0
TCA = 2026-01-01T00:00:00.000
MISS_DISTANCE = 0.125 [km]
RELATIVE_SPEED = 12.5 [km/s]
COLLISION_PROBABILITY = 1.2e-5
OBJECT1_OBJECT_DESIGNATOR = 25544
OBJECT2_OBJECT_DESIGNATOR = 99999
)");
    if (!parsed.ok || !parsed.record || parsed.record->object1_id != "25544" || parsed.record->object2_id != "99999" ||
        std::abs(parsed.record->miss_distance_m - 125.0) > 1e-12 || std::abs(parsed.record->relative_speed_m_s - 12'500.0) > 1e-12 ||
        !parsed.record->collision_probability || std::abs(*parsed.record->collision_probability - 1.2e-5) > 1e-15) return 1;
    if (nadir::astro::parse_cdm_kvn("CCSDS_CDM_VERS = 1.0").ok ||
        nadir::astro::parse_cdm_kvn("CCSDS_CDM_VERS = 1.0\nTCA = x\nOBJECT1_OBJECT_DESIGNATOR = 1\nOBJECT2_OBJECT_DESIGNATOR = 2\nMISS_DISTANCE = -1\nRELATIVE_SPEED = 1").ok ||
        nadir::astro::parse_cdm_kvn(std::string(1024U * 1024U + 1U, 'x')).ok) return 2;
    std::uint32_t random = 0x4c957f2dU;
    for (int sample = 0; sample < 512; ++sample) {
        const int length = static_cast<int>(random % 512U);
        random = random * 1664525U + 1013904223U;
        std::string fuzz;
        fuzz.reserve(length);
        for (int index = 0; index < length; ++index) {
            random = random * 1664525U + 1013904223U;
            fuzz.push_back(static_cast<char>(random >> 24U));
        }
        (void)nadir::astro::parse_cdm_kvn(fuzz);
    }
    return 0;
}
