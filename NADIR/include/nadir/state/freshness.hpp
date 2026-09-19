#pragma once

#include <nadir/time/instant.hpp>
#include <nadir/units/duration.hpp>

namespace nadir::state {

struct Freshness {
    time::TimeInstant reference_epoch{};
    units::Seconds maximum_age{0.0};
};

enum class FreshnessStatus {
    Unknown,
    Fresh,
    Stale
};

constexpr FreshnessStatus evaluate_freshness(
    const Freshness& freshness,
    time::TimeInstant now) noexcept {
    if (freshness.maximum_age.value <= 0.0) return FreshnessStatus::Unknown;
    if (now < freshness.reference_epoch) return FreshnessStatus::Fresh;

    const auto age_seconds =
        static_cast<long double>(now.tai_seconds) -
        static_cast<long double>(freshness.reference_epoch.tai_seconds) +
        (static_cast<long double>(now.nanoseconds) - freshness.reference_epoch.nanoseconds) /
            time::nanoseconds_per_second;
    return age_seconds > freshness.maximum_age.value
        ? FreshnessStatus::Stale
        : FreshnessStatus::Fresh;
}

} // namespace nadir::state
