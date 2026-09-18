#pragma once

#include <cstdint>
#include <chrono>
#include <string>
#include <string_view>

#include "nadir/time/time_scale_enum.hpp"

namespace nadir::time {

struct TimeState {
    // Primary: UTC in nanoseconds since Unix epoch (1970-01-01 00:00:00 UTC)
    int64_t utc_ns{0};

    // Monotonic time: nanoseconds since some arbitrary but consistent starting point
    // (typically system boot or process start), never goes backwards
    int64_t monotonic_ns{0};

    // UT1 - UTC in seconds (positive or negative)
    // Indicates Earth rotation deviation from atomic time
    double dut1{0.0};

    // Julian Date variants
    double jd_utc{0.0};     // Julian Date using UTC
    double jd_ut1{0.0};     // Julian Date using UT1
    double jd_tt{0.0};      // Julian Date using Terrestrial Time
    double jd_tdb{0.0};     // Julian Date using Barycentric Dynamical Time

    // Uncertainty in seconds (how precise this time is)
    double uncertainty_s{0.0};

    // Last update timestamp
    std::chrono::steady_clock::time_point last_update{};

    // CelesTrak/GST (Greenwich Sidereal Time) related
    double gst_rad{0.0};    // Greenwich Sidereal Time in radians
    double last_gst_update_ns{0};

    // Reference frame indicator
    std::string reference_frame{"UTC"};

    // Human-readable formatting helpers
    std::string utc_iso8601() const;
    std::string modified_julian_date() const;
    std::string time_scale_name() const;
};

// ============= Inline implementations =============

inline std::string TimeState::utc_iso8601() const {
    // Simplified: just indicate the scale and epoch
    // Full implementation would format actual datetime
    return scale_to_string(TimeScale::UTC) + " ns since epoch";
}

inline std::string TimeState::modified_julian_date() const {
    // MJD = JD - 2400000.5
    // Using UTC JD for reference
    double mjd = jd_utc - 2400000.5;
    return "MJD " + std::to_string(static_cast<int64_t>(mjd));
}

inline std::string TimeState::time_scale_name() const {
    return std::string(scale_to_string(TimeScale::UTC));
}

} // namespace nadir::time