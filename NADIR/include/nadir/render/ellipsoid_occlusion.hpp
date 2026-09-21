#pragma once

#include <cstdint>

#include <nadir/math/vec3.hpp>

namespace nadir::render {

struct Ellipsoid {
    double semi_major_m{};
    double semi_minor_m{};
};

struct EllipsoidOcclusion {
    bool enabled{};
    math::Vec3d center_m{};
    Ellipsoid ellipsoid{};
    std::uint64_t occluder_entity_id{};
};

[[nodiscard]] Ellipsoid wgs84_ellipsoid() noexcept;
[[nodiscard]] bool segment_intersects_ellipsoid(const math::Vec3d& origin,
                                                 const math::Vec3d& target,
                                                 const Ellipsoid& ellipsoid) noexcept;
[[nodiscard]] bool occluded_by_ellipsoid(const math::Vec3d& camera,
                                          const math::Vec3d& object,
                                          const Ellipsoid& ellipsoid) noexcept;

} // namespace nadir::render
