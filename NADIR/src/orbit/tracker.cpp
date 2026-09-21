#include <nadir/orbit/tracker.hpp>
#include <nadir/astro/sun.hpp>

#include <cmath>

namespace nadir::orbit {
TrackingResult track_omm(const TrackingRequest& request) {
    const auto propagated = propagate_sgp4(request.elements, request.minutes_since_epoch);
    if (!propagated) return {{}, {}, {}, {}, {}, Illumination::Invalid, propagated.error};
    const geo::StateVector teme_m{{propagated.state.position_km.x * 1000.0, propagated.state.position_km.y * 1000.0, propagated.state.position_km.z * 1000.0},
                                  {propagated.state.velocity_km_s.x * 1000.0, propagated.state.velocity_km_s.y * 1000.0, propagated.state.velocity_km_s.z * 1000.0}};
    const auto itrf = geo::teme_to_itrf(teme_m, request.jd_utc, request.eop);
    const auto geodetic = geo::ecef_to_geodetic(itrf.position);
    const auto topocentric = geo::observe_itrf(itrf, request.observer);
    const Vec3d satellite_km{itrf.position.x / 1000.0, itrf.position.y / 1000.0, itrf.position.z / 1000.0};
    const auto sun = request.sun_itrf_km.norm() > 0.0 ? request.sun_itrf_km : astro::sun_position_itrf_km(request.jd_utc, request.eop);
    const auto illumination = classify_earth_eclipse(satellite_km, sun);
    const double doppler = request.transmitted_hz > 0.0
        ? geo::doppler_observed_hz(request.transmitted_hz, topocentric.range_rate) : 0.0;
    return {propagated.state, itrf, geodetic, topocentric, doppler, illumination, Sgp4Error::None};
}
} // namespace nadir::orbit
