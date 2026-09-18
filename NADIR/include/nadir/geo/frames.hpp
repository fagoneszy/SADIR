#pragma once
#include <nadir/core/math.hpp>
#include <nadir/geo/eop.hpp>
#include <nadir/geo/wgs84.hpp>

namespace nadir::geo {

struct StateVector {
    Vec3 position;
    Vec3 velocity;
};

struct Topocentric {
    double east{};
    double north{};
    double up{};
    double azimuth_deg{};
    double elevation_deg{};
    double range{};
    double range_rate{};
};

double gmst_vallado(double jd_ut1);
StateVector teme_to_itrf(const StateVector& teme,double jd_utc,const EopRecord& eop);
Vec3 ecef_delta_to_enu(const Vec3& delta,const Geodetic& observer);
Topocentric observe_itrf(const StateVector& target,const Geodetic& observer);
double doppler_observed_hz(double transmitted_hz,double range_rate_m_s);

}
