#pragma once

namespace nadir::units {

struct Seconds {
    double value{};

    constexpr explicit Seconds(double v = 0.0) noexcept : value(v) {}
};

struct Minutes {
    double value{};

    constexpr explicit Minutes(double v = 0.0) noexcept : value(v) {}
};

struct Hours {
    double value{};

    constexpr explicit Hours(double v = 0.0) noexcept : value(v) {}
};

struct Days {
    double value{};

    constexpr explicit Days(double v = 0.0) noexcept : value(v) {}
};

constexpr Seconds to_seconds(Minutes value) noexcept {
    return Seconds{value.value * 60.0};
}

constexpr Seconds to_seconds(Hours value) noexcept {
    return Seconds{value.value * 3600.0};
}

constexpr Seconds to_seconds(Days value) noexcept {
    return Seconds{value.value * 86400.0};
}

constexpr Minutes to_minutes(Seconds value) noexcept {
    return Minutes{value.value / 60.0};
}

constexpr Hours to_hours(Seconds value) noexcept {
    return Hours{value.value / 3600.0};
}

constexpr Days to_days(Seconds value) noexcept {
    return Days{value.value / 86400.0};
}

} // namespace nadir::units
