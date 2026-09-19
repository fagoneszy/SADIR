#pragma once

namespace nadir::units {

struct Meters {
    double value{};

    constexpr explicit Meters(double v = 0.0) noexcept : value(v) {}
};

struct Kilometers {
    double value{};

    constexpr explicit Kilometers(double v = 0.0) noexcept : value(v) {}
};

constexpr Meters to_meters(Kilometers value) noexcept {
    return Meters{value.value * 1000.0};
}

constexpr Kilometers to_kilometers(Meters value) noexcept {
    return Kilometers{value.value / 1000.0};
}

constexpr Meters operator+(Meters a, Meters b) noexcept {
    return Meters{a.value + b.value};
}

constexpr Meters operator-(Meters a, Meters b) noexcept {
    return Meters{a.value - b.value};
}

constexpr Meters operator*(Meters value, double scalar) noexcept {
    return Meters{value.value * scalar};
}

constexpr Meters operator*(double scalar, Meters value) noexcept {
    return value * scalar;
}

constexpr Meters operator/(Meters value, double scalar) noexcept {
    return Meters{value.value / scalar};
}

constexpr bool operator==(Meters a, Meters b) noexcept {
    return a.value == b.value;
}

} // namespace nadir::units
