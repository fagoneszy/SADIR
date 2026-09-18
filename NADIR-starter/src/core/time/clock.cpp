#include "nadir/time/clock.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace nadir::time {

Clock::Clock() {
    start_monotonic_ = std::chrono::steady_clock::now();
    state_.last_update = start_monotonic_;
    state_.gst_rad = 0.0;
    gps_epoch_ns_ = 0; // Will be set via sync_with_gps_time
}

void Clock::tick() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_monotonic_).count();

    // Update monotonic
    state_.monotonic_ns = elapsed;

    // Update last_update for reference
    state_.last_update = now;
}

void Clock::set_utc_from_epoch(int64_t epoch_ns) {
    state_.utc_ns = epoch_ns;
    state_.last_update = std::chrono::steady_clock::now();
}

void Clock::set_monotonic_from_steady(std::chrono::steady_clock::time_point now) {
    state_.monotonic_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_monotonic_).count();
    state_.last_update = now;
}

void Clock::set_from_ntp_offset(int64_t ntp_offset_ns, int64_t /*ntp_receive_ns*/) {
    // Simple NTP offset storage
    ntp_offset_ns_ = ntp_offset_ns;
    ntp_last_update_ = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    // Apply to UTC
    state_.utc_ns += ntp_offset_ns;
}

TimeState Clock::get_state() const {
    return state_;
}

int64_t Clock::to_utc_ns() const {
    return state_.utc_ns;
}

double Clock::to_utc_seconds() const {
    return static_cast<double>(state_.utc_ns) / 1'000'000'000.0;
}

double Clock::to_ut1_minutes() const {
    // dut1 is in seconds; convert to minutes
    return dut1() / 60.0;
}

double Clock::greenwich_sidereal_time_rad() const {
    // Simplified GST calculation from UTC
    // GST = 2π * (0.7790572732640 + 1.00273781191135448 * (TT - T0))
    // Where T0 = 2000-01-01 12:00 TT
    // Using simplified version for demo
    double tt = to_utc_seconds() + 32.184; // Light-time correction placeholder
    double t = (tt - 2451545.0) / 36525.0; // Centuries since J2000
    // GST rad = 2π * (0.7790572732640 + 1.00273781191135448 * T)
    return 2.0 * M_PI * (0.7790572732640 + 1.00273781191135448 * t);
}

double Clock::apparent_sidereal_time_rad() const {
    // GST + equation of the equinoxes + nutation
    // For now return GST as approximation
    return greenwich_sidereal_time_rad();
}

bool Clock::is_valid() const {
    // UTC should be reasonable: after 1970, before year 2100
    double utc_years = to_utc_seconds() / (365.25 * 86400.0);
    return utc_years >= 1970.0 && utc_years < 2100.0;
}

std::chrono::system_clock::time_t Clock::to_c_time_t() const {
    return std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::time_point(
            std::chrono::seconds(state_.utc_ns / 1000)
        )
    );
}

void Clock::freeze() {
    frozen_ = true;
}

void Clock::unfreeze() {
    frozen_ = false;
    tick(); // Refresh state
}

bool Clock::is_frozen() const {
    return frozen_;
}

void Clock::sync_with_iers_eop(double dut1, double xp, double yp, double lod) {
    stored_dut1_ = dut1;
    stored_xp_ = xp;
    stored_yp_ = yp;
    stored_lod_ = lod;

    // Apply DUT1 to UTC -> UT1 conversion
    // UT1 = UTC + DUT1
    state_.dut1 = dut1;
}

void Clock::sync_with_gps_time(int64_t gps_week, int64_t gps_tow_ns) {
    // GPS epoch: 1980-01-06 00:00:00 UTC = 315964800 seconds since Unix epoch
    const int64_t GPS_EPOCH_NS = 315964800LL * 1'000'000'000LL;

    // Convert GPSTow to epoch ns
    int64_t gpst_epoch_ns = GPS_EPOCH_NS + (gps_week * 7 * 86400LL * 1'000'000'000LL) + gps_tow_ns;

    // GPS time does not have leap seconds; to convert to UTC we need leap second count
    // For now, simple conversion assuming 0 leap seconds (accurate until 2017)
    state_.utc_ns = gpst_epoch_ns - (37LL * 1'000'000'000LL); // Subtract accumulated leap seconds

    state_.last_update = std::chrono::steady_clock::now();
}
} // namespace nadir::time