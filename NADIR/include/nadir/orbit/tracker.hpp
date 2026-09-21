#pragma once

#include <nadir/astro/omm.hpp>
#include <nadir/geo/eop.hpp>
#include <nadir/geo/frames.hpp>
#include <nadir/orbit/eclipse.hpp>
#include <nadir/orbit/sgp4.hpp>

namespace nadir::orbit {

struct TrackingRequest {
    astro::OmmRecord elements;
    double minutes_since_epoch{};
    double jd_utc{};
    geo::EopRecord eop{};
    geo::Geodetic observer{};
    Vec3d sun_itrf_km{};
    double transmitted_hz{};
};

struct TrackingResult {
    State teme{};
    geo::StateVector itrf_m{};
    geo::Geodetic geodetic{};
    geo::Topocentric topocentric{};
    double doppler_hz{};
    Illumination illumination{Illumination::Invalid};
    Sgp4Error error{Sgp4Error::None};
    constexpr explicit operator bool() const noexcept { return error == Sgp4Error::None; }
};

TrackingResult track_omm(const TrackingRequest& request);

} // namespace nadir::orbit
