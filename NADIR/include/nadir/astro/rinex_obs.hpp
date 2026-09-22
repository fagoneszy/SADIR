#pragma once
#include <optional>
#include <string>
#include <vector>
namespace nadir::astro {
struct RinexObservationValue { std::string satellite_id; std::string code; std::optional<double> value; };
struct RinexObservationEpoch { std::string epoch; int flag{}; int satellite_count{}; std::vector<RinexObservationValue> observations; };
struct RinexObservationParseResult { bool ok{}; std::optional<std::vector<RinexObservationEpoch>> epochs; std::string error; };
// Parses bounded RINEX 3 epoch headers plus fixed-width observables declared by
// `SYS / # / OBS TYPES` header records. Unsupported event epochs are retained
// without observations.
RinexObservationParseResult parse_rinex_observation_epochs(const std::string& text);
} // namespace nadir::astro
