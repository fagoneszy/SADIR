#include <nadir/time/instant.hpp>

#include <limits>

namespace nadir::time {
namespace {

std::optional<TimeInstant> normalize(
    std::int64_t seconds,
    std::int64_t nanoseconds) noexcept {
    const auto quotient = nanoseconds / nanoseconds_per_second;
    const auto remainder = nanoseconds % nanoseconds_per_second;

    if ((quotient > 0 && seconds > std::numeric_limits<std::int64_t>::max() - quotient) ||
        (quotient < 0 && seconds < std::numeric_limits<std::int64_t>::min() - quotient)) {
        return std::nullopt;
    }
    seconds += quotient;

    if (remainder < 0) {
        if (seconds == std::numeric_limits<std::int64_t>::min()) {
            return std::nullopt;
        }
        --seconds;
        return TimeInstant{seconds, static_cast<std::int32_t>(remainder + nanoseconds_per_second)};
    }
    return TimeInstant{seconds, static_cast<std::int32_t>(remainder)};
}

} // namespace

std::optional<TimeInstant> make_instant(
    std::int64_t tai_seconds,
    std::int64_t nanoseconds) noexcept {
    return normalize(tai_seconds, nanoseconds);
}

std::optional<TimeDelta> make_delta(
    std::int64_t seconds,
    std::int64_t nanoseconds) noexcept {
    const auto normalized = normalize(seconds, nanoseconds);
    if (!normalized) {
        return std::nullopt;
    }
    return TimeDelta{normalized->tai_seconds, normalized->nanoseconds};
}

std::optional<TimeInstant> add(TimeInstant instant, TimeDelta delta) noexcept {
    if ((delta.seconds > 0 && instant.tai_seconds > std::numeric_limits<std::int64_t>::max() - delta.seconds) ||
        (delta.seconds < 0 && instant.tai_seconds < std::numeric_limits<std::int64_t>::min() - delta.seconds)) {
        return std::nullopt;
    }
    return normalize(instant.tai_seconds + delta.seconds,
                     static_cast<std::int64_t>(instant.nanoseconds) + delta.nanoseconds);
}

std::optional<TimeInstant> subtract(TimeInstant instant, TimeDelta delta) noexcept {
    if (delta.seconds == std::numeric_limits<std::int64_t>::min()) {
        return std::nullopt;
    }
    return add(instant, TimeDelta{-delta.seconds, -delta.nanoseconds});
}

} // namespace nadir::time
