#pragma once

#include <cstdint>
#include <vector>

#include <nadir/math/vec3.hpp>

namespace nadir::orbit {
struct ScreeningObject { std::uint64_t id{}; math::Vec3d position_m{}; math::Vec3d velocity_m_s{}; };
struct Conjunction { std::uint64_t first_id{}, second_id{}; double tca_seconds{}, miss_distance_m{}, relative_speed_m_s{}; };

// Constant-velocity coarse screen. Results are sorted by TCA then miss distance.
std::vector<Conjunction> screen_conjunctions(const std::vector<ScreeningObject>& objects,
                                              double horizon_seconds, double threshold_m);
} // namespace nadir::orbit
