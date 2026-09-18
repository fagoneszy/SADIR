#pragma once
#include <array>
#include <cmath>
#include <cstddef>

namespace nadir {

struct Vec2 {
    double x{};
    double y{};
};

struct Vec3 {
    double x{};
    double y{};
    double z{};

    Vec3 operator+(const Vec3& rhs) const { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
    Vec3 operator-(const Vec3& rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
    Vec3 operator*(double s) const { return {x * s, y * s, z * s}; }
};

inline double dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline double length(const Vec3& v) {
    return std::sqrt(dot(v, v));
}

inline Vec3 normalize(const Vec3& v) {
    const double l = length(v);
    if (l == 0.0) return {};
    return v * (1.0 / l);
}

struct Mat3 {
    std::array<double, 9> m{};

    static Mat3 identity() {
        return {{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}};
    }

    static Mat3 rotation_x(double a) {
        const double c = std::cos(a);
        const double s = std::sin(a);
        return {{1.0, 0.0, 0.0, 0.0, c, -s, 0.0, s, c}};
    }

    static Mat3 rotation_y(double a) {
        const double c = std::cos(a);
        const double s = std::sin(a);
        return {{c, 0.0, s, 0.0, 1.0, 0.0, -s, 0.0, c}};
    }

    static Mat3 rotation_z(double a) {
        const double c = std::cos(a);
        const double s = std::sin(a);
        return {{c, -s, 0.0, s, c, 0.0, 0.0, 0.0, 1.0}};
    }

    Vec3 operator*(const Vec3& v) const {
        return {
            m[0] * v.x + m[1] * v.y + m[2] * v.z,
            m[3] * v.x + m[4] * v.y + m[5] * v.z,
            m[6] * v.x + m[7] * v.y + m[8] * v.z
        };
    }

    Mat3 operator*(const Mat3& r) const {
        Mat3 out{};
        for (std::size_t row = 0; row < 3; ++row) {
            for (std::size_t col = 0; col < 3; ++col) {
                for (std::size_t k = 0; k < 3; ++k) {
                    out.m[row * 3 + col] += m[row * 3 + k] * r.m[k * 3 + col];
                }
            }
        }
        return out;
    }
};

constexpr double pi = 3.141592653589793238462643383279502884;
constexpr double deg_to_rad = pi / 180.0;
constexpr double rad_to_deg = 180.0 / pi;

}
