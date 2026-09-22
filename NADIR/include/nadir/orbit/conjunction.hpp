#pragma once

#include <cstdint>
#include <vector>

#include <nadir/math/vec3.hpp>
#include <nadir/state/covariance.hpp>

namespace nadir::orbit {
struct ScreeningObject { std::uint64_t id{}; math::Vec3d position_m{}; math::Vec3d velocity_m_s{}; };
struct Conjunction { std::uint64_t first_id{}, second_id{}; double tca_seconds{}, miss_distance_m{}, relative_speed_m_s{}; };

// Constant-velocity coarse screen. Results are sorted by TCA then miss distance.
std::vector<Conjunction> screen_conjunctions(const std::vector<ScreeningObject>& objects,
                                              double horizon_seconds, double threshold_m);

// Numerically integrates the combined 2D Gaussian covariance over the hard-
// body disk in the plane normal to relative_velocity_m_s. The relative
// position must be evaluated at the encounter epoch.
std::optional<double> collision_probability_encounter_plane(const math::Vec3d& relative_position_m,
                                                              const math::Vec3d& relative_velocity_m_s,
                                                              const state::Covariance6& first_covariance,
                                                              const state::Covariance6& second_covariance,
                                                              double hard_body_radius_m);
} // namespace nadir::orbit
