#pragma once
#include <optional>
#include <string>
#include <vector>
namespace nadir::astro {
struct RinexObservationEpoch { std::string epoch; int flag{}; int satellite_count{}; };
struct RinexObservationParseResult { bool ok{}; std::optional<std::vector<RinexObservationEpoch>> epochs; std::string error; };
// Parses bounded RINEX 3 observation epoch headers. Individual observables are
// deliberately retained for a later schema-aware measurement layer.
RinexObservationParseResult parse_rinex_observation_epochs(const std::string& text);
} // namespace nadir::astro
