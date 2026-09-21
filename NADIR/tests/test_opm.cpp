#include <nadir/astro/opm.hpp>
#include <string>
#include <cstdint>

int main() {
    const auto parsed=nadir::astro::parse_opm_kvn(R"(CCSDS_OPM_VERS = 3.0
OBJECT_NAME = TEST
OBJECT_ID = 2026-001A
CENTER_NAME = EARTH
REF_FRAME = GCRF
TIME_SYSTEM = UTC
EPOCH = 2026-01-01T00:00:00.000
X = 7000.0 [km]
Y = 0.0 [km]
Z = 0.0 [km]
X_DOT = 0.0 [km/s]
Y_DOT = 7.5 [km/s]
Z_DOT = 1.0 [km/s]
)");
    if (!parsed.ok || !parsed.record || parsed.record->state_m.position_m.x != 7'000'000.0 || parsed.record->state_m.velocity_m_s.y != 7'500.0) return 1;
    const auto round_trip=nadir::astro::parse_opm_kvn(nadir::astro::write_opm_kvn(*parsed.record));
    if (!round_trip.ok || !round_trip.record || round_trip.record->state_m.position_m.x != 7'000'000.0 || round_trip.record->state_m.velocity_m_s.y != 7'500.0) return 2;
    if (nadir::astro::parse_opm_kvn("OBJECT_NAME = missing").ok) return 3;
    if (nadir::astro::parse_opm_kvn(std::string(1024U * 1024U + 1U, 'x')).ok) return 4;
    std::uint32_t state=0x9e3779b9U;
    for (int sample=0; sample<512; ++sample) {
        std::string fuzz;
        const int length=static_cast<int>(state % 512U); state=state*1664525U+1013904223U;
        fuzz.reserve(length);
        for (int i=0; i<length; ++i) { state=state*1664525U+1013904223U; fuzz.push_back(static_cast<char>(state >> 24U)); }
        (void)nadir::astro::parse_opm_kvn(fuzz);
    }
    return 0;
}
