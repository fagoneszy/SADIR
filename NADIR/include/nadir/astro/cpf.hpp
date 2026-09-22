#pragma once
#include <nadir/math/vec3.hpp>
#include <optional>
#include <string>
#include <vector>
namespace nadir::astro {
struct CpfPosition { int direction{}; int modified_julian_date{}; double seconds_of_day_utc{}; int leap_second_flag{}; math::Vec3d position_m; };
struct CpfParseResult { bool ok{}; std::optional<std::vector<CpfPosition>> positions; std::string error; };
// Parses bounded ILRS CPF position records (type 10) after the required H1/H2/H9 headers.
CpfParseResult parse_cpf(const std::string& text);
} // namespace nadir::astro
