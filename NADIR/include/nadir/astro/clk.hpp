#pragma once
#include <optional>
#include <string>
#include <vector>
namespace nadir::astro {
struct ClkSample { std::string epoch; std::string satellite_id; double clock_bias_s{}; std::optional<double> clock_bias_sigma_s; };
struct ClkRecord { double version{}; std::vector<ClkSample> samples; };
struct ClkParseResult { bool ok{}; std::optional<ClkRecord> record; std::string error; };
// Parses bounded RINEX CLK satellite (AS) clock records.
ClkParseResult parse_rinex_clk(const std::string& text);
} // namespace nadir::astro
