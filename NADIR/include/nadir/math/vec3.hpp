#pragma once

#include <cmath>

namespace nadir::math {

struct Vec3d {
    double x{};
    double y{};
    double z{};

    constexpr Vec3d operator+(const Vec3d& rhs) const noexcept {
        return {
            x + rhs.x,
            y + rhs.y,
            z + rhs.z
        };
    }

    constexpr Vec3d operator-(const Vec3d& rhs) const noexcept {
        return {
            x - rhs.x,
            y - rhs.y,
            z - rhs.z
        };
    }

    constexpr Vec3d operator*(double s) const noexcept {
        return {
            x * s,
            y * s,
            z * s
        };
    }

    constexpr Vec3d operator/(double s) const noexcept {
        return {
            x / s,
            y / s,
            z / s
        };
    }

    constexpr Vec3d& operator+=(const Vec3d& rhs) noexcept {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    constexpr Vec3d& operator-=(const Vec3d& rhs) noexcept {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    constexpr Vec3d& operator*=(double s) noexcept {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    constexpr Vec3d& operator/=(double s) noexcept {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    constexpr double dot(const Vec3d& rhs) const noexcept {
        return
            x * rhs.x +
            y * rhs.y +
            z * rhs.z;
    }

    constexpr Vec3d cross(const Vec3d& rhs) const noexcept {
        return {
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        };
    }

    double norm() const noexcept {
        return std::sqrt(dot(*this));
    }

    double norm_sq() const noexcept {
        return dot(*this);
    }

    Vec3d normalized() const noexcept {
        const double n = norm();
        if (n <= 1.0e-15) {
            return {};
        }
        return *this / n;
    }

    Vec3d& normalize() noexcept {
        const double n = norm();
        if (n <= 1.0e-15) {
            *this = {};
            return *this;
        }
        *this /= norm();
        return *this;
    }
};

constexpr Vec3d operator*(double s, const Vec3d& v) noexcept {
    return v * s;
}

}