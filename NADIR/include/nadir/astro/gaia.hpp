#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
namespace nadir::astro {
struct GaiaSource { std::uint64_t source_id{}; double right_ascension_deg{}; double declination_deg{}; double g_magnitude{}; };
struct GaiaParseResult { bool ok{}; std::optional<std::vector<GaiaSource>> sources; std::string error; };
// Parses the source_id, ra, dec and phot_g_mean_mag CSV export subset.
GaiaParseResult parse_gaia_csv(const std::string& text);
} // namespace nadir::astro
