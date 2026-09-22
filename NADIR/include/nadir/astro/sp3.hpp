#pragma once

#include <nadir/math/vec3.hpp>

#include <optional>
#include <string>
#include <vector>

namespace nadir::astro {
struct Sp3Sample { std::string epoch; std::string satellite_id; math::Vec3d position_m; std::optional<double> clock_offset_s; };
struct Sp3Record { char version{}; std::string time_system; std::vector<Sp3Sample> samples; };
struct Sp3ParseResult { bool ok{}; std::optional<Sp3Record> record; std::string error; };
// Parses bounded SP3 position records. Coordinates are km on the wire and
// normalized to metres; clocks are microseconds and normalized to seconds.
Sp3ParseResult parse_sp3(const std::string& text);
} // namespace nadir::astro
