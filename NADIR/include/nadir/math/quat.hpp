#pragma once

#include <nadir/math/vec3.hpp>

namespace nadir::math {

struct Quatd {
    double w{1.0};
    double x{};
    double y{};
    double z{};

    static Quatd identity() noexcept;

    static Quatd from_axis_angle(const Vec3d& axis, double radians) noexcept;

    Quatd normalized() const noexcept;

    Quatd conjugate() const noexcept;

    Quatd operator*(const Quatd& rhs) const noexcept;

    Quatd& operator*=(const Quatd& rhs) noexcept;

    Vec3d rotate(const Vec3d& v) const noexcept;
};

}