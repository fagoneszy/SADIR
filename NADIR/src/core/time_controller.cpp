#include "nadir/core/time_controller.hpp"
#include "nadir/core/time.hpp"
#include <chrono>

namespace nadir::core {

TimeState TimeController::now() const {
    const auto unix_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return TimeState{
        .unix_ns = unix_ns,
        .jd_utc = 2440587.5 + static_cast<double>(unix_ns) / 86400.0e9,
        .mjd_utc = static_cast<double>(unix_ns) / 86400000.0 - 150187.5
    };
}

void TimeController::freeze() {
    frozen_ = true;
    frozen_utc_ns_ = now().unix_ns;
}

void TimeController::unfreeze() {
    frozen_ = false;
    // Recalculate now after unfreeze
    (void)now();
}

void TimeController::set_utc_ns(std::int64_t value) {
    // Store the requested UTC value for offset calculation
    offset_ns_ = value - frozen_utc_ns_;
}

bool TimeController::frozen() const noexcept {
    return frozen_;
}

std::int64_t TimeController::offset_ns() const noexcept {
    return offset_ns_;
}

} // namespace nadir::core