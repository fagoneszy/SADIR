#include "nadir/core/time_controller.hpp"
#include "nadir/core/time.hpp"
#include <chrono>
#include <cmath>

namespace nadir::core {

namespace {
std::int64_t wall_utc_ns() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}
} // namespace

TimeController::TimeController() {
    anchor(wall_utc_ns());
}

std::int64_t TimeController::running_utc_ns() const {
    const auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now() - anchor_steady_).count();
    return anchor_utc_ns_ + static_cast<std::int64_t>(std::llround(static_cast<double>(elapsed) * rate_));
}

void TimeController::anchor(std::int64_t utc_ns) {
    anchor_utc_ns_ = utc_ns;
    anchor_steady_ = std::chrono::steady_clock::now();
}

TimeState TimeController::now() const {
    return time_from_unix_ns(frozen_ ? frozen_utc_ns_ : running_utc_ns());
}

void TimeController::freeze() {
    if (frozen_) return;
    frozen_utc_ns_ = running_utc_ns();
    frozen_ = true;
}

void TimeController::unfreeze() {
    if (!frozen_) return;
    anchor(frozen_utc_ns_);
    frozen_ = false;
}

void TimeController::set_utc_ns(std::int64_t value) {
    offset_ns_ = value - wall_utc_ns();
    if (frozen_) frozen_utc_ns_ = value;
    else anchor(value);
}

void TimeController::set_offset_ns(std::int64_t value) {
    offset_ns_ = value;
    set_utc_ns(wall_utc_ns() + value);
    offset_ns_ = value;
}

void TimeController::set_rate(double value) {
    if (!std::isfinite(value)) return;
    if (!frozen_) anchor(running_utc_ns());
    rate_ = value;
}

bool TimeController::frozen() const noexcept {
    return frozen_;
}

std::int64_t TimeController::offset_ns() const noexcept {
    return offset_ns_;
}

double TimeController::rate() const noexcept {
    return rate_;
}

} // namespace nadir::core
