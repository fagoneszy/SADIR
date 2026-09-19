#pragma once

#include <cstdint>

namespace nadir::orbit {

struct Epoch {
    std::int64_t utc_ns{};
    double jd_utc{};
    double jd_ut1{};
    double jd_tt{};
};

}