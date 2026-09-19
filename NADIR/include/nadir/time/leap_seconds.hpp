#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include <nadir/time/utc.hpp>

namespace nadir::time {

// An entry becomes effective at 00:00:00 UTC on its date.
struct LeapSecondEntry {
    std::int32_t year{};
    std::uint8_t month{};
    std::uint8_t day{};
    std::int32_t tai_minus_utc{};
};

class LeapSecondTable {
public:
    LeapSecondTable();
    explicit LeapSecondTable(std::vector<LeapSecondEntry> entries);

    [[nodiscard]] std::optional<std::int32_t> tai_minus_utc(
        const UtcDateTime& utc) const noexcept;
    [[nodiscard]] bool is_positive_leap_second(
        const UtcDateTime& utc) const noexcept;
    [[nodiscard]] const std::vector<LeapSecondEntry>& entries() const noexcept;

private:
    std::vector<LeapSecondEntry> entries_;
};

} // namespace nadir::time
