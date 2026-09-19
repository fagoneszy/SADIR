#pragma once

#include <nadir/math/quat.hpp>
#include <nadir/math/vec3.hpp>

namespace nadir::render {

struct Camera {
    nadir::math::Vec3d target{};
    nadir::math::Quatd orientation{nadir::math::Quatd::identity()};
    double distance{3.2};
    double fov_deg{60.0};
    double near_plane{0.01};
    double far_plane{1000.0};

    void yaw(double radians) noexcept;
    void pitch(double radians) noexcept;
    void roll(double radians) noexcept;

    void zoom(double delta) noexcept;

    nadir::math::Vec3d position() const noexcept;

    nadir::math::Vec3d world_to_view(const nadir::math::Vec3d& world) const noexcept;
};

}