#pragma once

#include <cstdint>
#include <cmath>
#include <limits>

#include <nadir/astro/omm.hpp>
#include <nadir/orbit/state.hpp>
#include <nadir/orbit/sgp4.hpp>

namespace nadir::orbit {

constexpr double pi = 3.141592653589793238462643383279502884;
constexpr double two_pi = 2.0 * pi;

constexpr double deg_to_rad(double deg) noexcept {
    return deg * pi / 180.0;
}

constexpr double rev_day_to_rad_min(double rev_day) noexcept {
    return rev_day * two_pi / 1440.0;
}

struct Sgp4Constants {
    double mu;
    double radius_earth_km;
    double j2;
    double j3;
    double j4;
    double j3oj2;
    double xke;
    double tumin;
};

inline Sgp4Constants wgs72_constants() noexcept {
    constexpr double mu = 398600.8;
    constexpr double re = 6378.135;
    constexpr double j2 = 0.001082616;
    constexpr double j3 = -0.00000253881;
    constexpr double j4 = -0.00000165597;

    const double xke = 60.0 / std::sqrt((re * re * re) / mu);
    const double tumin = 1.0 / xke;

    return {
        mu,
        re,
        j2,
        j3,
        j4,
        j3 / j2,
        xke,
        tumin
    };
}

enum class OpsMode {
    Afspc,
    Improved
};

struct SatRec {
    std::uint64_t satnum{};

    double epoch_jd{};

    double bstar{};
    double ecco{};
    double argpo{};
    double inclo{};
    double mo{};
    double no_kozai{};
    double nodeo{};

    double cc1{};
    double cc4{};
    double cc5{};
    double d2{};
    double d3{};
    double d4{};
    double eta{};
    double argpdot{};
    double mdot{};
    double nodedot{};
    double nodecf{};
    double t2cof{};
    double t3cof{};
    double t4cof{};
    double t5cof{};

    double x1mth2{};
    double x7thm1{};
    double xlcof{};
    double aycof{};

    bool simplified{};
    bool deep_space{};
    bool method{};

    double t{};
    double m{};
    double n{};
    double e{};
    double a{};
    double i{};
    double omgadf{};
    double xnode{};
    double omega{};
    double xmp{};
    double tsince{};

    double em{};
    double xmdf{};
    double xnddf{};
    double xnddt{};

    double dndt{};
    double d2201{};
    double d2211{};
    double d3210{};
    double d3222{};
    double d4410{};
    double d4422{};
    double d5220{};
    double d5232{};
    double d5421{};
    double d5433{};
    double del1{};
    double del2{};
    double del3{};
    double fasx2{};
    double fasx4{};
    double fasx6{};
    double xlamo{};
    double fact2{};
    double fact3{};
    double fact4{};
    double fact5{};
    double xfact{};
    double xli{};
    double xni{};
    double atime{};
    double xnodce{};
    double xnodcf{};
    double xmdot{};
    double omgdot{};
    double xnodot{};
    double xll{};
    double xnoh{};
    double toth{};
    double c1{};
    double c4{};
    double c5{};
    double d2201_2{};
    double d2211_2{};
    double d3210_2{};
    double d3222_2{};
    double d4410_2{};
    double d4422_2{};
    double d5220_2{};
    double d5232_2{};
    double d5421_2{};
    double d5433_2{};
    double xnoddf{};
    double xnodp{};
    double aodp{};
    double epoch{};
    double ds50{};

    double irez{};
    double d2201_{};
    double d2211_{};
    double d3210_{};
    double d3222_{};
    double d4410_{};
    double d4422_{};
    double d5220_{};
    double d5232_{};
    double d5421_{};
    double d5433_{};
    double dedt{};
    double del1_{};
    double del2_{};
    double del3_{};
    double didt{};
    double dmdt{};
    double dnodt{};
    double domdt{};
    double e3{};
    double ee2{};
    double peo{};
    double pgho{};
    double pho{};
    double pinco{};
    double plo{};
    double se2{};
    double se3{};
    double sgh2{};
    double sgh3{};
    double sgh4{};
    double sh2{};
    double sh3{};
    double si2{};
    double si3{};
    double sl2{};
    double sl3{};
    double sl4{};
    double gsto{};
    double xgh2{};
    double xgh3{};
    double xgh4{};
    double xh2{};
    double xh3{};
    double xi2{};
    double xi3{};
    double xl2{};
    double xl3{};
    double xl4{};
    double zmol{};
    double zmos{};

    double omgcof{};
    double xmcof{};
    double delmo{};
    double sinmao{};
    double sinio{};
    double cosio{};
    double cosio2{};
    double eccsq{};
    double omeosq{};
    double rteosq{};
    double con41{};
    double con42{};
    double x3thm1{};
    double x7thm1_{};
    double xnoddf_{};
    double xnodp_{};
    double aodp_{};
    double epoch_{};
    double ds50_{};
};

struct Sgp4Elements {
    std::uint64_t norad{};
    double epoch_jd{};
    double bstar{};
    double eccentricity{};
    double inclination_rad{};
    double raan_rad{};
    double arg_perigee_rad{};
    double mean_anomaly_rad{};
    double mean_motion_rad_min{};
};

SatRec build_satrec(const Sgp4Elements& elem, const Sgp4Constants& constants, OpsMode opsmode);

Sgp4Result propagate_sgp4_rec(
    SatRec& satrec,
    double tsince,
    const Sgp4Constants& constants
);

void sgp4init(
    OpsMode opsmode,
    const Sgp4Elements& elem,
    SatRec& satrec,
    const Sgp4Constants& constants
);

void sgp4(
    SatRec& satrec,
    double tsince,
    Vec3d& position_km,
    Vec3d& velocity_km_s,
    const Sgp4Constants& constants
);

void dscom(
    double epoch,
    double ep,
    double argpp,
    double tc,
    double inclp,
    double nodep,
    double np,
    double& sinim,
    double& cosim,
    double& emsq,
    double& snodm,
    double& cnodm,
    double& sini2,
    double& cosi2,
    double& emsq2,
    double& eccm,
    double& ms,
    double& s1,
    double& s2,
    double& s3,
    double& s4,
    double& s5,
    double& s6,
    double& s7,
    double& ss1,
    double& ss2,
    double& ss3,
    double& ss4,
    double& ss5,
    double& ss6,
    double& ss7,
    double& sz1,
    double& sz2,
    double& sz3,
    double& sz11,
    double& sz12,
    double& sz13,
    double& sz21,
    double& sz22,
    double& sz23,
    double& sz31,
    double& sz32,
    double& sz33,
    double& zmol,
    double& zmos
);

void dsinit(
    OpsMode opsmode,
    double& xmdot,
    double& omgdot,
    double& xnodot,
    SatRec& satrec,
    const Sgp4Constants& constants
);

void dspace(
    double tsince,
    SatRec& satrec,
    const Sgp4Constants& constants
);

void dpper(
    SatRec& satrec,
    const Sgp4Constants& constants
);

Sgp4Elements omm_to_elements(const astro::OmmRecord& rec);

} // namespace nadir::orbit