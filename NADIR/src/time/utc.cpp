#include <nadir/time/utc.hpp>

namespace nadir::time {

bool is_valid_utc_date_time(const UtcDateTime& value) noexcept {
    if (value.month < 1 || value.month > 12 || value.day < 1 || value.hour > 23 ||
        value.minute > 59 || value.second > 60 || value.nanosecond >= 1'000'000'000U) {
        return false;
    }

    const bool leap_year = value.year % 4 == 0 &&
        (value.year % 100 != 0 || value.year % 400 == 0);
    constexpr unsigned days_per_month[] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    const unsigned month_days = days_per_month[value.month - 1] +
        (value.month == 2 && leap_year ? 1 : 0);
    return value.day <= month_days;
}

} // namespace nadir::time
