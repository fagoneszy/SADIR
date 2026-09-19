#include <nadir/time/leap_seconds.hpp>

#include <algorithm>

namespace nadir::time {
namespace {

constexpr bool date_less(const UtcDateTime& a, const LeapSecondEntry& b) noexcept {
    if (a.year != b.year) return a.year < b.year;
    if (a.month != b.month) return a.month < b.month;
    return a.day < b.day;
}

constexpr std::int64_t days_from_civil(int year, unsigned month, unsigned day) noexcept {
    year -= month <= 2;
    const int era = (year >= 0 ? year : year - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(year - era * 400);
    const unsigned day_of_year = (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;
    const unsigned day_of_era = yoe * 365 + yoe / 4 - yoe / 100 + day_of_year;
    return static_cast<std::int64_t>(era) * 146097 + day_of_era - 719468;
}

} // namespace

LeapSecondTable::LeapSecondTable() : LeapSecondTable({
    {1972, 1, 1, 10}, {1972, 7, 1, 11}, {1973, 1, 1, 12}, {1974, 1, 1, 13},
    {1975, 1, 1, 14}, {1976, 1, 1, 15}, {1977, 1, 1, 16}, {1978, 1, 1, 17},
    {1979, 1, 1, 18}, {1980, 1, 1, 19}, {1981, 7, 1, 20}, {1982, 7, 1, 21},
    {1983, 7, 1, 22}, {1985, 7, 1, 23}, {1988, 1, 1, 24}, {1990, 1, 1, 25},
    {1991, 1, 1, 26}, {1992, 7, 1, 27}, {1993, 7, 1, 28}, {1994, 7, 1, 29},
    {1996, 1, 1, 30}, {1997, 7, 1, 31}, {1999, 1, 1, 32}, {2006, 1, 1, 33},
    {2009, 1, 1, 34}, {2012, 7, 1, 35}, {2015, 7, 1, 36}, {2017, 1, 1, 37}
}) {}

LeapSecondTable::LeapSecondTable(std::vector<LeapSecondEntry> entries)
    : entries_(std::move(entries)) {
    std::sort(entries_.begin(), entries_.end(), [](const auto& a, const auto& b) {
        if (a.year != b.year) return a.year < b.year;
        if (a.month != b.month) return a.month < b.month;
        return a.day < b.day;
    });
}

std::optional<std::int32_t> LeapSecondTable::tai_minus_utc(
    const UtcDateTime& utc) const noexcept {
    if (!is_valid_utc_date_time(utc) || utc.second == 60 || entries_.empty()) {
        return std::nullopt;
    }
    std::optional<std::int32_t> result;
    for (const auto& entry : entries_) {
        if (date_less(utc, entry)) break;
        result = entry.tai_minus_utc;
    }
    return result;
}

bool LeapSecondTable::is_positive_leap_second(const UtcDateTime& utc) const noexcept {
    if (!is_valid_utc_date_time(utc) || utc.hour != 23 || utc.minute != 59 || utc.second != 60) {
        return false;
    }

    for (std::size_t index = 1; index < entries_.size(); ++index) {
        const auto& current = entries_[index];
        const auto& previous = entries_[index - 1];
        if (current.tai_minus_utc != previous.tai_minus_utc + 1) continue;

        if (days_from_civil(utc.year, utc.month, utc.day) + 1 ==
            days_from_civil(current.year, current.month, current.day)) {
            return true;
        }
    }
    return false;
}

const std::vector<LeapSecondEntry>& LeapSecondTable::entries() const noexcept {
    return entries_;
}

} // namespace nadir::time
