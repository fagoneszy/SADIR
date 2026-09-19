#include <nadir/time/time_context.hpp>

#include <limits>

namespace nadir::time {
JulianDate julian_date_from_tai_parts(
    std::int64_t tai_seconds,
    std::int32_t nanoseconds,
    std::int64_t offset_nanoseconds) noexcept;

namespace {

constexpr std::int64_t seconds_per_day = 86'400;

constexpr std::int64_t days_from_civil(int year, unsigned month, unsigned day) noexcept {
    year -= month <= 2;
    const int era = (year >= 0 ? year : year - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(year - era * 400);
    const unsigned day_of_year = (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;
    const unsigned day_of_era = yoe * 365 + yoe / 4 - yoe / 100 + day_of_year;
    return static_cast<std::int64_t>(era) * 146097 + day_of_era - 719468;
}

UtcDateTime civil_from_days(std::int64_t days) noexcept {
    days += 719468;
    const auto era = (days >= 0 ? days : days - 146096) / 146097;
    const auto day_of_era = static_cast<unsigned>(days - era * 146097);
    const auto year_of_era = (day_of_era - day_of_era / 1460 + day_of_era / 36524 - day_of_era / 146096) / 365;
    int year = static_cast<int>(year_of_era) + static_cast<int>(era) * 400;
    const auto day_of_year = day_of_era - (365 * year_of_era + year_of_era / 4 - year_of_era / 100);
    const auto month_prime = (5 * day_of_year + 2) / 153;
    const auto day = day_of_year - (153 * month_prime + 2) / 5 + 1;
    const auto month = month_prime + (month_prime < 10 ? 3 : -9);
    year += month <= 2;
    return UtcDateTime{year, static_cast<std::uint8_t>(month), static_cast<std::uint8_t>(day)};
}

std::optional<std::int64_t> unix_seconds(const UtcDateTime& utc) noexcept {
    const auto days = days_from_civil(utc.year, utc.month, utc.day);
    if (days > std::numeric_limits<std::int64_t>::max() / seconds_per_day ||
        days < std::numeric_limits<std::int64_t>::min() / seconds_per_day) {
        return std::nullopt;
    }
    return days * seconds_per_day + utc.hour * 3600 + utc.minute * 60 + (utc.second == 60 ? 59 : utc.second);
}

std::optional<std::int64_t> add_seconds(
    std::int64_t value,
    std::int64_t offset) noexcept {
    if ((offset > 0 && value > std::numeric_limits<std::int64_t>::max() - offset) ||
        (offset < 0 && value < std::numeric_limits<std::int64_t>::min() - offset)) {
        return std::nullopt;
    }
    return value + offset;
}

UtcDateTime from_unix_seconds(std::int64_t value, std::int32_t nanoseconds) noexcept {
    auto days = value / seconds_per_day;
    auto second_of_day = value % seconds_per_day;
    if (second_of_day < 0) {
        second_of_day += seconds_per_day;
        --days;
    }
    auto result = civil_from_days(days);
    result.hour = static_cast<std::uint8_t>(second_of_day / 3600);
    second_of_day %= 3600;
    result.minute = static_cast<std::uint8_t>(second_of_day / 60);
    result.second = static_cast<std::uint8_t>(second_of_day % 60);
    result.nanosecond = static_cast<std::uint32_t>(nanoseconds);
    return result;
}

} // namespace

TimeContext::TimeContext(LeapSecondTable leap_seconds)
    : leap_seconds_(std::move(leap_seconds)) {}

std::optional<TimeInstant> TimeContext::from_utc(const UtcDateTime& utc) const noexcept {
    if (!is_valid_utc_date_time(utc)) return std::nullopt;
    const auto seconds = unix_seconds(utc);
    if (!seconds) return std::nullopt;

    if (utc.second == 60) {
        if (!leap_seconds_.is_positive_leap_second(utc)) return std::nullopt;
        if (*seconds == std::numeric_limits<std::int64_t>::max()) return std::nullopt;
        const auto offset_after = leap_seconds_.tai_minus_utc(
            from_unix_seconds(*seconds + 1, 0));
        if (!offset_after) return std::nullopt;
        const auto tai_seconds = add_seconds(*seconds, *offset_after);
        return tai_seconds ? make_instant(*tai_seconds, utc.nanosecond) : std::nullopt;
    }

    const auto offset = leap_seconds_.tai_minus_utc(utc);
    if (!offset) return std::nullopt;
    const auto tai_seconds = add_seconds(*seconds, *offset);
    return tai_seconds ? make_instant(*tai_seconds, utc.nanosecond) : std::nullopt;
}

std::optional<UtcDateTime> TimeContext::to_utc(TimeInstant instant) const noexcept {
    // A leap second is represented by the SI second immediately before a new offset takes effect.
    for (const auto& entry : leap_seconds_.entries()) {
        const UtcDateTime candidate{entry.year, entry.month, entry.day};
        auto before = from_unix_seconds(*unix_seconds(candidate) - 1, 0);
        auto leap_label = before;
        leap_label.second = 60;
        if (!leap_seconds_.is_positive_leap_second(leap_label)) continue;
        const auto boundary = from_utc(candidate);
        if (boundary && instant.tai_seconds == boundary->tai_seconds - 1) {
            auto result = before;
            result.second = 60;
            result.nanosecond = static_cast<std::uint32_t>(instant.nanoseconds);
            return result;
        }
    }

    // Find the UTC offset whose effective interval contains this TAI instant.
    for (std::int64_t probe = instant.tai_seconds - 37; probe <= instant.tai_seconds - 10; ++probe) {
        const auto utc = from_unix_seconds(probe, instant.nanoseconds);
        const auto offset = leap_seconds_.tai_minus_utc(utc);
        if (offset && probe + *offset == instant.tai_seconds) return utc;
    }
    return std::nullopt;
}

JulianDate TimeContext::to_jd_tai(TimeInstant instant) const noexcept {
    return julian_date_from_tai_parts(instant.tai_seconds, instant.nanoseconds, 0);
}

JulianDate TimeContext::to_jd_tt(TimeInstant instant) const noexcept {
    return julian_date_from_tai_parts(instant.tai_seconds, instant.nanoseconds, tt_minus_tai_nanoseconds);
}

JulianDate TimeContext::to_jd_gps(TimeInstant instant) const noexcept {
    return julian_date_from_tai_parts(instant.tai_seconds, instant.nanoseconds, -tai_minus_gps_nanoseconds);
}

std::optional<JulianDate> TimeContext::to_julian_date(
    TimeInstant instant,
    TimeScale scale) const noexcept {
    switch (scale) {
    case TimeScale::TAI: return to_jd_tai(instant);
    case TimeScale::TT: return to_jd_tt(instant);
    case TimeScale::GPS: return to_jd_gps(instant);
    case TimeScale::UTC:
    case TimeScale::UT1:
    case TimeScale::TDB: return std::nullopt;
    }
    return std::nullopt;
}

} // namespace nadir::time
