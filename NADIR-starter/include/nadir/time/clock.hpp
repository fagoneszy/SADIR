#pragma once

#include <cstdint>
#include <chrono>
#include <array>
#include <optional>
#include <string>

#include "nadir/time/time_state.hpp"
#include "nadir/time/time_scale_enum.hpp"

namespace nadir::time {

class Clock {
public:
    Clock();
    ~Clock() = default;

    // Tick the clock forward (called each frame/iteration)
    void tick();

    // Set time from various sources
    void set_utc_from_epoch(int64_t epoch_ns);
    void set_monotonic_from_steady(std::chrono::steady_clock::time_point now);
    void set_from_ntp_offset(int64_t ntp_offset_ns, int64_t ntp_receive_ns);

    // Get current time state
    TimeState get_state() const;

    // Time scaling conversions
    int64_t to_utc_ns() const;
    double to_utc_seconds() const;
    double to_ut1_minutes() const;

    // Reference frame transformations
    double greenwich_sidereal_time_rad() const;
    double apparent_sidereal_time_rad() const;

    // Time queries
    bool is_valid() const;
    std::chrono::system_clock::to_time_t to_c_time_t() const;

    // Control
    void freeze();
    void unfreeze();
    bool is_frozen() const;

    // Synchronization with external sources
    void sync_with_iers_eop(double dut1, double xp, double yp, double lod);
    void sync_with_gps_time(int64_t gps_week, int64_t gps_tow_ns);

private:
    TimeState state_;
    bool frozen_{false};
    std::chrono::steady_clock::time_point start_monotonic_;

    // NTP-related
    int64_t ntp_offset_ns_{0};    // Offset from NTP measurement
    int64_t ntp_last_update_{0};  // When we last measured NTP

    // IERS EOP data
    double stored_dut1_{0.0};
    double stored_xp_{0.0};     // Polar motion X (arcseconds)
    double stored_yp_{0.0};     // Polar motion Y (arcseconds)
    double stored_lod_{0.0};    // Length of Day (seconds)

    // GPS time base
    int64_t gps_epoch_ns_{0};    // GPS epoch: 1980-01-06 00:00:00 UTC in ns
};

} // namespace nadir::time