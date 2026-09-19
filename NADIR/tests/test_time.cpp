#include <nadir/time/time_context.hpp>

#include <cmath>
#include <cstdint>
#include <limits>

int main() {
    using namespace nadir::time;
    const TimeContext context;

    const auto normalized = make_instant(1, 1'500'000'000LL);
    if (!normalized || normalized->tai_seconds != 2 || normalized->nanoseconds != 500'000'000) return 1;
    const auto negative = make_instant(1, -500'000'000LL);
    if (!negative || negative->tai_seconds != 0 || negative->nanoseconds != 500'000'000) return 2;

    const UtcDateTime normal{2017, 1, 1, 0, 0, 0, 123'456'789};
    const auto tai = context.from_utc(normal);
    if (!tai || tai->tai_seconds != 1'483'228'837LL || tai->nanoseconds != 123'456'789) return 3;
    const auto round_trip = tai ? context.to_utc(*tai) : std::nullopt;
    if (!round_trip || round_trip->year != normal.year || round_trip->month != normal.month ||
        round_trip->day != normal.day || round_trip->hour != normal.hour ||
        round_trip->minute != normal.minute || round_trip->second != normal.second ||
        round_trip->nanosecond != normal.nanosecond) return 4;

    const UtcDateTime before{2016, 12, 31, 23, 59, 59, 0};
    const UtcDateTime leap{2016, 12, 31, 23, 59, 60, 0};
    const UtcDateTime after{2017, 1, 1, 0, 0, 0, 0};
    const auto tai_before = context.from_utc(before);
    const auto tai_leap = context.from_utc(leap);
    const auto tai_after = context.from_utc(after);
    if (!tai_before || !tai_leap || !tai_after ||
        tai_leap->tai_seconds != tai_before->tai_seconds + 1 ||
        tai_after->tai_seconds != tai_leap->tai_seconds + 1) return 5;
    const auto leap_round_trip = context.to_utc(*tai_leap);
    if (!leap_round_trip || leap_round_trip->second != 60 || leap_round_trip->day != 31) return 6;
    if (context.from_utc(UtcDateTime{2017, 1, 1, 23, 59, 60, 0})) return 7;

    const auto jd_tai = context.to_jd_tai(*tai);
    const auto jd_tt = context.to_jd_tt(*tai);
    const auto jd_gps = context.to_jd_gps(*tai);
    if (jd_tai.part2 < 0.0 || jd_tai.part2 >= 1.0 ||
        jd_tt.part2 < 0.0 || jd_tt.part2 >= 1.0 ||
        std::abs((jd_tt.part2 - jd_tai.part2) * 86'400.0 - 32.184) > 1.0e-9) return 8;
    if (std::abs((jd_tai.part2 - jd_gps.part2) * 86'400.0 - 19.0) > 1.0e-9) return 9;
    if (context.to_julian_date(*tai, TimeScale::UT1)) return 10;

    const auto delta = make_delta(1, 500'000'000);
    const auto advanced = delta ? add(TimeInstant{10, 750'000'000}, *delta) : std::nullopt;
    if (!advanced || advanced->tai_seconds != 12 || advanced->nanoseconds != 250'000'000) return 11;
    if (make_instant(std::numeric_limits<std::int64_t>::max(), 1'000'000'000LL)) return 12;
    if (add(TimeInstant{std::numeric_limits<std::int64_t>::max(), 0}, TimeDelta{1, 0})) return 13;

    return 0;
}
