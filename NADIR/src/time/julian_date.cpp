#include <nadir/time/julian_date.hpp>

#include <cstdint>

namespace nadir::time {

inline constexpr std::int64_t seconds_per_day = 86'400;
inline constexpr std::int64_t nanoseconds_per_day = 86'400'000'000'000LL;
inline constexpr double tai_epoch_julian_date = 2'440'587.5;

JulianDate julian_date_from_tai_parts(
    std::int64_t tai_seconds,
    std::int32_t nanoseconds,
    std::int64_t offset_nanoseconds) noexcept {
    auto days = tai_seconds / seconds_per_day;
    auto seconds_of_day = tai_seconds % seconds_per_day;
    if (seconds_of_day < 0) {
        seconds_of_day += seconds_per_day;
        --days;
    }

    auto nanoseconds_of_day = seconds_of_day * 1'000'000'000LL + nanoseconds + offset_nanoseconds;
    if (nanoseconds_of_day >= nanoseconds_per_day) {
        nanoseconds_of_day -= nanoseconds_per_day;
        ++days;
    } else if (nanoseconds_of_day < 0) {
        nanoseconds_of_day += nanoseconds_per_day;
        --days;
    }

    return {
        tai_epoch_julian_date + static_cast<double>(days),
        static_cast<double>(nanoseconds_of_day) / static_cast<double>(nanoseconds_per_day)
    };
}

} // namespace nadir::time
