#pragma once

#include <string_view>

namespace nadir::time {

enum class TimeScale {
    UTC,     // Coordinated Universal Time
    TAI,     // International Atomic Time
    TT,      // Terrestrial Time
    UT1,     // Universal Time 1
    GPS,     // GPS Time
    MONOTONIC // Monotonic time (system boot monotonic)
};

inline std::string_view scale_to_string(TimeScale scale) {
    switch (scale) {
        case TimeScale::UTC:   return "UTC";
        case TimeScale::TAI:   return "TAI";
        case TimeScale::TT:    return "TT";
        case TimeScale::UT1:   return "UT1";
        case TimeScale::GPS:   return "GPS";
        case TimeScale::MONOTONIC: return "MONOTONIC";
        default: return "UNKNOWN";
    }
}

} // namespace nadir::time