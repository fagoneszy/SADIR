#pragma once

namespace nadir::units {

struct MetersPerSecond {
    double value{};

    constexpr explicit MetersPerSecond(double v = 0.0) noexcept : value(v) {}
};

struct KilometersPerSecond {
    double value{};

    constexpr explicit KilometersPerSecond(double v = 0.0) noexcept : value(v) {}
};

constexpr MetersPerSecond to_meters_per_second(KilometersPerSecond value) noexcept {
    return MetersPerSecond{value.value * 1000.0};
}

constexpr KilometersPerSecond to_kilometers_per_second(MetersPerSecond value) noexcept {
    return KilometersPerSecond{value.value / 1000.0};
}

} // namespace nadir::units
