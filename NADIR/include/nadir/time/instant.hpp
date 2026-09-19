#pragma once

#include <cstdint>
#include <optional>

namespace nadir::time {

// Continuous TAI seconds from NADIR's epoch: 1970-01-01T00:00:00 TAI.
struct TimeInstant {
    std::int64_t tai_seconds{};
    std::int32_t nanoseconds{};

    constexpr auto operator<=>(const TimeInstant&) const noexcept = default;
};

struct TimeDelta {
    std::int64_t seconds{};
    std::int32_t nanoseconds{};
};

inline constexpr std::int32_t nanoseconds_per_second = 1'000'000'000;

std::optional<TimeInstant> make_instant(
    std::int64_t tai_seconds,
    std::int64_t nanoseconds = 0) noexcept;

std::optional<TimeDelta> make_delta(
    std::int64_t seconds,
    std::int64_t nanoseconds = 0) noexcept;

std::optional<TimeInstant> add(TimeInstant instant, TimeDelta delta) noexcept;
std::optional<TimeInstant> subtract(TimeInstant instant, TimeDelta delta) noexcept;

} // namespace nadir::time
