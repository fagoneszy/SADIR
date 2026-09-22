#include <nadir/astro/tdm.hpp>

#include <cmath>
#include <string>

int main() {
    const auto parsed = nadir::astro::parse_tdm_kvn(R"(CCSDS_TDM_VERS = 2.0
TIME_SYSTEM = UTC
DATA_START
EPOCH = 2026-01-01T00:00:00.000
RANGE = 1234.5 [km]
RANGE_RATE = -1.25 [km/s]
DOPPLER_INSTANTANEOUS = 145800123.0 [Hz]
DATA_STOP
)");
    if (!parsed.ok || !parsed.record || parsed.record->observations.size() != 1) return 1;
    const auto& observation = parsed.record->observations.front();
    if (!observation.range_m || !observation.range_rate_m_s || !observation.doppler_hz ||
        std::abs(*observation.range_m - 1'234'500.0) > 1e-12 || std::abs(*observation.range_rate_m_s + 1'250.0) > 1e-12 ||
        std::abs(*observation.doppler_hz - 145800123.0) > 1e-9) return 2;
    const auto round_trip = nadir::astro::parse_tdm_kvn(nadir::astro::write_tdm_kvn(*parsed.record));
    if (!round_trip.ok || !round_trip.record || !round_trip.record->observations.front().range_m ||
        *round_trip.record->observations.front().range_m != *observation.range_m) return 3;
    if (nadir::astro::parse_tdm_kvn("CCSDS_TDM_VERS = 2.0\nTIME_SYSTEM = UTC\nDATA_START\nRANGE = 1\nDATA_STOP").ok ||
        nadir::astro::parse_tdm_kvn(std::string(1024U * 1024U + 1U, 'x')).ok) return 4;
    return 0;
}
