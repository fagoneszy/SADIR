#pragma once

#include <cstdint>

namespace nadir::time {

struct UtcDateTime {
    std::int32_t year{};
    std::uint8_t month{};
    std::uint8_t day{};
    std::uint8_t hour{};
    std::uint8_t minute{};
    std::uint8_t second{};
    std::uint32_t nanosecond{};
};

bool is_valid_utc_date_time(const UtcDateTime& value) noexcept;

} // namespace nadir::time
