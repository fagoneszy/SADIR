#include <nadir/orbit/tracker.hpp>
#include <cmath>
int main() {
    nadir::orbit::TrackingRequest request{};
    request.elements.norad_cat_id = 25544; request.elements.epoch = "2008-09-20T12:25:39.004000";
    request.elements.mean_motion_rev_day = 15.72125391; request.elements.eccentricity = 0.0006703;
    request.elements.inclination_deg = 51.6416; request.elements.raan_deg = 247.4627;
    request.elements.arg_pericenter_deg = 130.5360; request.elements.mean_anomaly_deg = 325.0288; request.elements.bstar = -0.000011606;
    request.jd_utc = 2454730.01781255; request.observer = {0.0, 0.0, 0.0}; request.sun_itrf_km = {149597870.7, 0.0, 0.0}; request.transmitted_hz = 145800000.0;
    const auto result = nadir::orbit::track_omm(request);
    if (!result || !std::isfinite(result.geodetic.latitude_deg) || !std::isfinite(result.topocentric.range) || result.topocentric.range <= 0.0) return 1;
    return result.teme.frame == nadir::orbit::Frame::TEME ? 0 : 2;
}
