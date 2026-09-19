#pragma once

#include <numbers>

namespace nadir::units {

struct Radians {
    double value{};

    constexpr explicit Radians(double v = 0.0) noexcept : value(v) {}
};

struct Degrees {
    double value{};

    constexpr explicit Degrees(double v = 0.0) noexcept : value(v) {}
};

constexpr Radians to_radians(Degrees value) noexcept {
    return Radians{value.value * std::numbers::pi / 180.0};
}

constexpr Degrees to_degrees(Radians value) noexcept {
    return Degrees{value.value * 180.0 / std::numbers::pi};
}

constexpr Radians operator+(Radians a, Radians b) noexcept {
    return Radians{a.value + b.value};
}

constexpr Radians operator-(Radians a, Radians b) noexcept {
    return Radians{a.value - b.value};
}

} // namespace nadir::units
