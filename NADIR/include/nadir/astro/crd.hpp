#pragma once
#include <optional>
#include <string>
#include <vector>
namespace nadir::astro {
struct CrdNormalPoint { double seconds_of_day{}, time_of_flight_s{}, window_s{}; std::string system_id; int epoch_event{}, raw_ranges{}, detector_channel{}; };
struct CrdParseResult { bool ok{}; std::optional<std::vector<CrdNormalPoint>> normal_points; std::string error; };
CrdParseResult parse_crd_normal_points(const std::string& text);
} // namespace nadir::astro
