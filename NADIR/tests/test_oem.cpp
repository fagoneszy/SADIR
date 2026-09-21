#include <nadir/astro/oem.hpp>

#include <string>

int main() {
    const auto parsed = nadir::astro::parse_oem_kvn(R"(CCSDS_OEM_VERS = 2.0
META_START
OBJECT_NAME = TEST
OBJECT_ID = 2026-001A
CENTER_NAME = EARTH
REF_FRAME = GCRF
TIME_SYSTEM = UTC
META_STOP
DATA_START
2026-01-01T00:00:00.000 7000.0 0.0 0.0 0.0 7.5 1.0
2026-01-01T00:01:00.000 6999.0 450.0 60.0 -0.5 7.4 1.0
DATA_STOP
)");
    if (!parsed.ok || !parsed.record || parsed.record->samples.size() != 2 ||
        parsed.record->samples[0].state_m.position_m.x != 7'000'000.0 || parsed.record->samples[0].state_m.velocity_m_s.y != 7'500.0) return 1;
    const auto round_trip = nadir::astro::parse_oem_kvn(nadir::astro::write_oem_kvn(*parsed.record));
    if (!round_trip.ok || !round_trip.record || round_trip.record->samples != parsed.record->samples) return 2;
    if (nadir::astro::parse_oem_kvn("CCSDS_OEM_VERS = 2.0").ok ||
        nadir::astro::parse_oem_kvn(std::string(1024U * 1024U + 1U, 'x')).ok) return 3;
    return 0;
}
