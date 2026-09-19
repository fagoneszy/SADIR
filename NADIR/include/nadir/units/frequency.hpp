#pragma once

namespace nadir::units {

struct Hertz {
    double value{};

    constexpr explicit Hertz(double v = 0.0) noexcept : value(v) {}
};

struct Kilohertz {
    double value{};

    constexpr explicit Kilohertz(double v = 0.0) noexcept : value(v) {}
};

struct Megahertz {
    double value{};

    constexpr explicit Megahertz(double v = 0.0) noexcept : value(v) {}
};

constexpr Hertz to_hertz(Kilohertz value) noexcept {
    return Hertz{value.value * 1000.0};
}

constexpr Hertz to_hertz(Megahertz value) noexcept {
    return Hertz{value.value * 1'000'000.0};
}

} // namespace nadir::units
