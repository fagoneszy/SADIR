#pragma once
#include <optional>
#include <string>
#include <vector>
namespace nadir::astro {
struct RinexNavRecord { std::string satellite_id; std::string epoch; double clock_bias_s{}; double clock_drift_s_s{}; double clock_drift_rate_s_s2{}; };
struct RinexNavParseResult { bool ok{}; std::optional<std::vector<RinexNavRecord>> records; std::string error; };
// Parses bounded RINEX 3 navigation record first lines. Full broadcast-orbit
// propagation remains a separate concern.
RinexNavParseResult parse_rinex_navigation(const std::string& text);
} // namespace nadir::astro
