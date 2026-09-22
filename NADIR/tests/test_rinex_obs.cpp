#include <nadir/astro/rinex_obs.hpp>
#include <cstdint>
#include <string>

int main() {
    const auto parsed = nadir::astro::parse_rinex_observation_epochs(
        "     3.04           O                   RINEX VERSION / TYPE\n"
        "G    6 C1C L1C D1C S1C C2W                             SYS / # / OBS TYPES\n"
        "      L2W                                               SYS / # / OBS TYPES\n"
        "                                                            END OF HEADER\n"
        "> 2026 01 01 00 00 00.0000000  0  2\n"
        "G01         123.0           456.0             1.0             2.0             3.0\n"
        "              4.0\n"
        "G02         789.0                \n"
        "             10.0\n");
    if (!parsed.ok || !parsed.epochs || parsed.epochs->size() != 1 || (*parsed.epochs)[0].satellite_count != 2 ||
        (*parsed.epochs)[0].epoch != "2026-01-01T00:00:00.000" || (*parsed.epochs)[0].observations.size() != 12 ||
        !(*parsed.epochs)[0].observations[0].value || *(*parsed.epochs)[0].observations[0].value != 123.0 ||
        (*parsed.epochs)[0].observations[5].code != "L2W" || !(*parsed.epochs)[0].observations[5].value ||
        *(*parsed.epochs)[0].observations[5].value != 4.0 || (*parsed.epochs)[0].observations[7].value) return 1;
    if (nadir::astro::parse_rinex_observation_epochs(
        "     3.04           O                   RINEX VERSION / TYPE\n"
        "                                                            END OF HEADER\n"
        "> 2026 13 01 00 00 00 0 1\n").ok) return 2;
    if (nadir::astro::parse_rinex_observation_epochs(std::string(16U * 1024U * 1024U + 1U, 'x')).ok) return 3;
    std::uint32_t random = 0xc2b2ae35U;
    for (int sample{}; sample < 512; ++sample) {
        const auto length = random % 512U;
        random = random * 1664525U + 1013904223U;
        std::string fuzz;
        fuzz.reserve(length);
        for (std::uint32_t index{}; index < length; ++index) {
            random = random * 1664525U + 1013904223U;
            fuzz.push_back(static_cast<char>(random >> 24U));
        }
        (void)nadir::astro::parse_rinex_observation_epochs(fuzz);
    }
    return 0;
}
