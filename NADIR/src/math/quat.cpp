#include <nadir/math/quat.hpp>
#include <cmath>

namespace nadir::math {

Quatd Quatd::operator*(const Quatd& rhs) const noexcept {
    return {
        w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
        w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
        w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x,
        w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w
    };
}

Quatd Quatd::normalized() const noexcept {
    const double n = std::sqrt(w * w + x * x + y * y + z * z);
    if (n <= 1.0e-15) {
        return {1.0, 0.0, 0.0, 0.0};
    }
    const double inv = 1.0 / n;
    return {w * inv, x * inv, y * inv, z * inv};
}

Quatd Quatd::conjugate() const noexcept {
    return {w, -x, -y, -z};
}

Quatd& Quatd::operator*=(const Quatd& rhs) noexcept {
    *this = *this * rhs;
    return *this;
}

Quatd Quatd::from_axis_angle(const nadir::math::Vec3d& axis, double radians) noexcept {
    const double half = radians * 0.5;
    const double s = std::sin(half);
    const nadir::math::Vec3d a = axis.normalized();
    return {
        std::cos(half),
        a.x * s,
        a.y * s,
        a.z * s
    };
}

nadir::math::Vec3d Quatd::rotate(const nadir::math::Vec3d& v) const noexcept {
    const Quatd p{0.0, v.x, v.y, v.z};
    const Quatd q = normalized();
    const Quatd result = q * p * q.conjugate();
    return {result.x, result.y, result.z};
}

}