#pragma once
#include <chrono>
#include <cstdint>
#include <string>

namespace nadir::core {

struct TimeState {
    std::int64_t unix_ns{};
    double jd_utc{};
    double mjd_utc{};
};

TimeState now_utc();
std::string iso8601_utc(std::int64_t unix_ns);
std::string compact_utc(std::int64_t unix_ns);

}
