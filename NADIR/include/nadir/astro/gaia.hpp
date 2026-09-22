#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
namespace nadir::astro {
struct GaiaSource {
    std::uint64_t source_id{};
    double right_ascension_deg{};
    double declination_deg{};
    double g_magnitude{};
    double reference_epoch_year{2016.0};
    std::optional<double> parallax_mas;
    std::optional<double> proper_motion_ra_mas_per_year;
    std::optional<double> proper_motion_dec_mas_per_year;
};
struct GaiaSkyPosition { double right_ascension_deg{}; double declination_deg{}; };
struct GaiaParseResult { bool ok{}; std::optional<std::vector<GaiaSource>> sources; std::string error; };
// Parses the source_id, ra, dec and phot_g_mean_mag subset, plus optional
// ref_epoch, parallax, pmra and pmdec Gaia CSV columns.
GaiaParseResult parse_gaia_csv(const std::string& text);
std::optional<GaiaSkyPosition> propagate_gaia_linear(const GaiaSource& source, double epoch_year) noexcept;
} // namespace nadir::astro
