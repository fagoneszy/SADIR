#include <nadir/geo/frames.hpp>
#include <algorithm>
#include <cmath>

namespace nadir::geo {

static constexpr double two_pi=2.0*pi;
static constexpr double arcsec_to_rad=deg_to_rad/3600.0;
static constexpr double c_m_s=299792458.0;

static double wrap(double x) {
    x=std::fmod(x,two_pi);
    return x<0.0?x+two_pi:x;
}

double gmst_vallado(double jd_ut1) {
    const double t=(jd_ut1-2451545.0)/36525.0;
    double s=-6.2e-6*t*t*t+0.093104*t*t+(876600.0*3600.0+8640184.812866)*t+67310.54841;
    return wrap(s*deg_to_rad/240.0);
}

static Mat3 polar_motion(double xp,double yp) {
    const double cx=std::cos(xp);
    const double sx=std::sin(xp);
    const double cy=std::cos(yp);
    const double sy=std::sin(yp);
    return {{cx,0.0,-sx,sx*sy,cy,cx*sy,sx*cy,-sy,cx*cy}};
}

StateVector teme_to_itrf(const StateVector& teme,double jd_utc,const EopRecord& eop) {
    const double jd_ut1=jd_utc+eop.dut1_s/86400.0;
    const double gst=gmst_vallado(jd_ut1);
    const Mat3 st=Mat3::rotation_z(-gst);
    const Vec3 rpef=st*teme.position;
    const Vec3 vrot=st*teme.velocity;
    const double omega_e=omega*(1.0-eop.lod_ms/86400000.0);
    const Vec3 cross_omega{-omega_e*rpef.y,omega_e*rpef.x,0.0};
    const Vec3 vpef=vrot-cross_omega;
    const Mat3 pm=polar_motion(eop.xp_arcsec*arcsec_to_rad,eop.yp_arcsec*arcsec_to_rad);
    return {pm*rpef,pm*vpef};
}

Vec3 ecef_delta_to_enu(const Vec3& d,const Geodetic& observer) {
    const double lat=observer.latitude_deg*deg_to_rad;
    const double lon=observer.longitude_deg*deg_to_rad;
    const double slat=std::sin(lat);
    const double clat=std::cos(lat);
    const double slon=std::sin(lon);
    const double clon=std::cos(lon);
    return {
        -slon*d.x+clon*d.y,
        -slat*clon*d.x-slat*slon*d.y+clat*d.z,
        clat*clon*d.x+clat*slon*d.y+slat*d.z
    };
}

Topocentric observe_itrf(const StateVector& target,const Geodetic& observer) {
    const Vec3 obs=geodetic_to_ecef(observer);
    const Vec3 dr=target.position-obs;
    const Vec3 dv=target.velocity;
    const Vec3 enu=ecef_delta_to_enu(dr,observer);
    const double horizontal=std::hypot(enu.x,enu.y);
    const double range=length(dr);
    double az=std::atan2(enu.x,enu.y)*rad_to_deg;
    if (az<0.0) az+=360.0;
    const double el=std::atan2(enu.z,horizontal)*rad_to_deg;
    const double rr=range>0.0?dot(dr,dv)/range:0.0;
    return {enu.x,enu.y,enu.z,az,el,range,rr};
}

double doppler_observed_hz(double transmitted_hz,double range_rate_m_s) {
    return transmitted_hz*(1.0-range_rate_m_s/c_m_s);
}

}
