#include <nadir/orbit/ground_track.hpp>

#include <cmath>

int main() {
    nadir::orbit::GroundTrackRequest request{};
    request.elements.object_name = "ISS"; request.elements.norad_cat_id = 25544;
    request.elements.epoch = "2008-09-20T12:25:39.004000";
    request.elements.mean_motion_rev_day = 15.72125391; request.elements.eccentricity = 0.0006703;
    request.elements.inclination_deg = 51.6416; request.elements.raan_deg = 247.4627;
    request.elements.arg_pericenter_deg = 130.5360; request.elements.mean_anomaly_deg = 325.0288;
    request.elements.bstar = -0.000011606; request.epoch_jd_utc = 2454730.01781255;
    request.step_minutes = 1.0; request.samples = 180;
    const auto track = nadir::orbit::ground_track(request);
    if (track.size() != request.samples) return 1;
    for (const auto& point : track) {
        if (!std::isfinite(point.latitude_deg) || !std::isfinite(point.longitude_deg) || !std::isfinite(point.altitude_m)) return 2;
        if (point.latitude_deg < -90.0 || point.latitude_deg > 90.0 || point.longitude_deg < -180.0 || point.longitude_deg > 180.0) return 3;
    }
    for (std::size_t i = 1; i < track.size(); ++i) {
        const auto difference = std::abs(track[i].longitude_deg - track[i - 1].longitude_deg);
        if (difference > 180.0 && track[i].segment != track[i - 1].segment + 1) return 4;
        if (difference <= 180.0 && track[i].segment != track[i - 1].segment) return 5;
    }
    return 0;
}
