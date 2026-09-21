#pragma once

#include <cstddef>
#include <vector>

#include <nadir/astro/omm.hpp>
#include <nadir/geo/eop.hpp>

namespace nadir::orbit {

// A point in a geographic ground track. segment changes at an anti-meridian
// crossing, so consumers never draw a false line across the map.
struct GroundTrackPoint {
    double minutes_since_epoch{};
    double latitude_deg{};
    double longitude_deg{};
    double altitude_m{};
    std::size_t segment{};
};

struct GroundTrackRequest {
    astro::OmmRecord elements;
    double epoch_jd_utc{};
    double start_minutes{};
    double step_minutes{1.0};
    std::size_t samples{};
    geo::EopRecord eop{};
};

std::vector<GroundTrackPoint> ground_track(const GroundTrackRequest& request);

} // namespace nadir::orbit
