#include <nadir/orbit/ground_track.hpp>

#include <nadir/geo/frames.hpp>
#include <nadir/geo/wgs84.hpp>
#include <nadir/orbit/sgp4.hpp>

#include <cmath>

namespace nadir::orbit {

std::vector<GroundTrackPoint> ground_track(const GroundTrackRequest& request) {
    std::vector<GroundTrackPoint> points;
    if (request.samples == 0 || !std::isfinite(request.epoch_jd_utc) ||
        !std::isfinite(request.start_minutes) || !std::isfinite(request.step_minutes) ||
        request.step_minutes <= 0.0) return points;
    points.reserve(request.samples);
    std::size_t segment = 0;
    double previous_longitude = 0.0;
    for (std::size_t i = 0; i < request.samples; ++i) {
        const double minutes = request.start_minutes + static_cast<double>(i) * request.step_minutes;
        const auto state = propagate_sgp4(request.elements, minutes);
        if (!state) continue;
        const geo::StateVector teme_m{
            {state.state.position_km.x * 1000.0, state.state.position_km.y * 1000.0, state.state.position_km.z * 1000.0},
            {state.state.velocity_km_s.x * 1000.0, state.state.velocity_km_s.y * 1000.0, state.state.velocity_km_s.z * 1000.0}};
        const double jd_utc = request.epoch_jd_utc + minutes / 1440.0;
        const auto geodetic = geo::ecef_to_geodetic(geo::teme_to_itrf(teme_m, jd_utc, request.eop).position);
        if (!points.empty() && std::abs(geodetic.longitude_deg - previous_longitude) > 180.0) ++segment;
        points.push_back({minutes, geodetic.latitude_deg, geodetic.longitude_deg, geodetic.altitude_m, segment});
        previous_longitude = geodetic.longitude_deg;
    }
    return points;
}

} // namespace nadir::orbit
