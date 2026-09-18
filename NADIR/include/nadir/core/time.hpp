#pragma once
#include <cstdint>
#include <string>

namespace nadir::core {

struct TimeState {
    std::int64_t unix_ns{};
    double jd_utc{};
    double mjd_utc{};
    int tai_minus_utc_s{};
    double jd_tai{};
    double jd_tt{};
    double dut1_s{};
    double jd_ut1{};
};

int tai_minus_utc(std::int64_t unix_ns);
TimeState time_from_unix_ns(std::int64_t unix_ns,double dut1_s=0.0);
TimeState now_utc(double dut1_s=0.0);
std::string iso8601_utc(std::int64_t unix_ns);
std::string compact_utc(std::int64_t unix_ns);

}
