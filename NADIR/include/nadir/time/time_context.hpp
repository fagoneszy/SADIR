#pragma once

#include <optional>

#include <nadir/time/instant.hpp>
#include <nadir/time/julian_date.hpp>
#include <nadir/time/leap_seconds.hpp>
#include <nadir/time/scale.hpp>

namespace nadir::time {

inline constexpr std::int64_t tt_minus_tai_nanoseconds = 32'184'000'000LL;
inline constexpr std::int64_t tai_minus_gps_nanoseconds = 19'000'000'000LL;

class TimeContext {
public:
    explicit TimeContext(LeapSecondTable leap_seconds = {});

    [[nodiscard]] std::optional<TimeInstant> from_utc(
        const UtcDateTime& utc) const noexcept;
    [[nodiscard]] std::optional<UtcDateTime> to_utc(
        TimeInstant instant) const noexcept;

    [[nodiscard]] JulianDate to_jd_tai(TimeInstant instant) const noexcept;
    [[nodiscard]] JulianDate to_jd_tt(TimeInstant instant) const noexcept;
    [[nodiscard]] JulianDate to_jd_gps(TimeInstant instant) const noexcept;
    [[nodiscard]] std::optional<JulianDate> to_julian_date(
        TimeInstant instant,
        TimeScale scale) const noexcept;

private:
    LeapSecondTable leap_seconds_;
};

} // namespace nadir::time
