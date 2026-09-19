#include <nadir/orbit/sgp4_internal.hpp>
#include <cmath>
#include <limits>

namespace nadir::orbit {

static constexpr double qoms2t = 1.88027916e-9;
static constexpr double s = 1.01222928;
static constexpr double ck2 = 0.5 * 0.001082616;
static constexpr double ck4 = -0.00165597 * 0.375;
static constexpr double ae = 1.0;
static constexpr double qoms24 = 1.88027916e-9;

void getgravconst(
    const Sgp4Constants& constants,
    double& tumin,
    double& mu,
    double& radius_earth_km,
    double& xke,
    double& j2,
    double& j3,
    double& j4,
    double& j3oj2
) {
    tumin = constants.tumin;
    mu = constants.mu;
    radius_earth_km = constants.radius_earth_km;
    xke = constants.xke;
    j2 = constants.j2;
    j3 = constants.j3;
    j4 = constants.j4;
    j3oj2 = constants.j3oj2;
}

void initl(
    double satn,
    double epoch,
    double& bstar,
    double& ecco,
    double& argpo,
    double& inclo,
    double& mo,
    double& no_kozai,
    double& nodeo,
    const Sgp4Constants& constants,
    double& ainv,
    double& ao,
    double& con41,
    double& con42,
    double& cosio,
    double& cosio2,
    double& eccsq,
    double& omeosq,
    double& posq,
    double& rp,
    double& rteosq,
    double& sinio,
    double& gsto,
    double& no_unkozai,
    bool& method,
    bool& isimp
) {
    double ak = std::pow(constants.xke / no_kozai, 2.0 / 3.0);
    double d1 = 0.75 * constants.j2 * (3.0 * std::cos(inclo) * std::cos(inclo) - 1.0) / (1.0 - ecco * ecco);
    double del1 = d1 / (ak * ak);
    double adel = ak * (1.0 - del1 * del1 - del1 * (1.0 / 3.0 + 134.0 * del1 * del1 / 81.0));
    double del = d1 / (adel * adel);
    no_unkozai = no_kozai / (1.0 + del);
    ao = std::pow(constants.xke / no_unkozai, 2.0 / 3.0);
    ainv = 1.0 / ao;
    double a1 = ao * (1.0 - del);
    double d2 = 0.5 * a1 * a1 * d1;
    double po = a1 * (1.0 - ecco * ecco);
    rp = po / (1.0 + ecco);
    posq = po;
    if (rp < (1.0 + constants.j2)) {
        posq = (1.0 + constants.j2) * (1.0 + constants.j2);
        isimp = true;
        return;
    }

    sinio = std::sin(inclo);
    cosio = std::cos(inclo);
    cosio2 = cosio * cosio;
    eccsq = ecco * ecco;
    omeosq = 1.0 - eccsq;
    rteosq = std::sqrt(omeosq);
    con41 = -1.5 * constants.j2 * sinio * sinio * (3.0 * cosio2 - 1.0);
    con42 = -1.5 * constants.j2 * cosio2;
    gsto = std::fmod(99.6909833 + 360.0 * 0.98564735 * (epoch - 2451545.0), 360.0);
    gsto = deg_to_rad(gsto);
    isimp = false;
}

void sgp4init(
    OpsMode opsmode,
    const Sgp4Elements& elem,
    SatRec& satrec,
    const Sgp4Constants& constants
) {
    satrec.satnum = elem.norad;
    satrec.epoch_jd = elem.epoch_jd;
    satrec.bstar = elem.bstar;
    satrec.ecco = elem.eccentricity;
    satrec.argpo = elem.arg_perigee_rad;
    satrec.inclo = elem.inclination_rad;
    satrec.mo = elem.mean_anomaly_rad;
    satrec.no_kozai = elem.mean_motion_rad_min;
    satrec.nodeo = elem.raan_rad;

    double ainv, ao, con41, con42, cosio, cosio2, eccsq, omeosq, posq, rp, rteosq, sinio, gsto, no_unkozai;
    bool method = false;
    satrec.deep_space = false;
    bool isimp = false;

    initl(
        static_cast<double>(elem.norad),
        elem.epoch_jd,
        satrec.bstar,
        satrec.ecco,
        satrec.argpo,
        satrec.inclo,
        satrec.mo,
        satrec.no_kozai,
        satrec.nodeo,
        constants,
        ainv, ao, con41, con42, cosio, cosio2, eccsq, omeosq, posq, rp, rteosq, sinio, gsto, no_unkozai, method, isimp
    );

    if (isimp) {
        satrec.simplified = true;
        satrec.no_kozai = 0.0;
        satrec.method = method;
        return;
    }

    satrec.simplified = false;
    satrec.method = method;

    double a1 = ao;
    double d1 = 0.75 * constants.j2 * (3.0 * cosio * cosio - 1.0) / (1.0 - satrec.ecco * satrec.ecco);
    double del1 = d1 / (a1 * a1);
    double adel = a1 * (1.0 - del1 * del1 - del1 * (1.0 / 3.0 + 134.0 * del1 * del1 / 81.0));
    double del = d1 / (adel * adel);
    no_unkozai = satrec.no_kozai / (1.0 + del);
    ao = std::pow(constants.xke / no_unkozai, 2.0 / 3.0);
    ainv = 1.0 / ao;

    double po = ao * (1.0 - satrec.ecco * satrec.ecco);
    rp = po / (1.0 + satrec.ecco);

    if (rp < (1.0 + constants.j2)) {
        satrec.no_kozai = 0.0;
        return;
    }

    double x1mth2 = 1.0 - cosio * cosio;
    satrec.x1mth2 = x1mth2;
    satrec.x7thm1 = 7.0 * cosio * cosio - 1.0;

    double c2 = constants.j2 * x1mth2;
    double c3 = 0.5 * constants.j2 * (3.0 * cosio * cosio - 1.0);
    double c4 = constants.j4;
    double c5 = constants.j2 * constants.j2;

    double cc1 = satrec.bstar * constants.xke * ao * (1.0 - satrec.ecco) * (1.0 - satrec.ecco);
    satrec.cc1 = cc1;
    double cc4 = 2.0 * satrec.no_kozai * ao * ao * satrec.bstar * x1mth2 / 3.0;
    satrec.cc4 = cc4;
    double cc5 = 2.0 * constants.xke * ao * ao * satrec.bstar * (1.0 - satrec.ecco) / 3.0;
    satrec.cc5 = cc5;

    double d2 = 0.5 * a1 * a1 * d1;
    double d3 = 0.5 * c2 * a1 * a1;
    double d4 = 0.25 * c2 * c2 * a1 * a1 * a1;
    satrec.d2 = d2;
    satrec.d3 = d3;
    satrec.d4 = d4;

    satrec.t2cof = 2.0 * d2 * constants.xke * no_unkozai;
    satrec.t3cof = 3.0 * d3 * constants.xke * no_unkozai;
    satrec.t4cof = 4.0 * d4 * constants.xke * no_unkozai;
    satrec.t5cof = 5.0 * d4 * constants.xke * no_unkozai;

    satrec.eta = satrec.ecco * sinio;
    satrec.argpdot = 1.5 * constants.j2 * (3.0 * cosio * cosio - 1.0) / (1.0 - satrec.ecco * satrec.ecco);
    satrec.mdot = satrec.no_kozai;
    satrec.nodedot = -1.5 * constants.j2 * cosio / (1.0 - satrec.ecco * satrec.ecco);

    satrec.xnodcf = 3.5 * constants.j2 * cosio / (1.0 - satrec.ecco * satrec.ecco);
    satrec.nodecf = satrec.xnodcf;

    double x1mth2_sq = x1mth2 * x1mth2;
    satrec.xlcof = 0.125 * constants.j2 * x1mth2_sq * (13.0 - 78.0 * cosio * cosio + 137.0 * cosio * cosio * cosio * cosio);
    satrec.aycof = 0.25 * constants.j2 * sinio * (7.0 - 14.0 * cosio * cosio);

    double xnodp = no_unkozai;
    double aodp = ao;
    double t = 0.0;

    satrec.xnodp = xnodp;
    satrec.aodp = aodp;
    satrec.t = t;

    double temp = 1.5 * constants.j2 * (3.0 * cosio * cosio - 1.0) / (1.0 - satrec.ecco * satrec.ecco);
    satrec.argpdot = temp;
    satrec.nodedot = -1.5 * constants.j2 * cosio / (1.0 - satrec.ecco * satrec.ecco);
    satrec.mdot = xnodp + 1.5 * constants.j2 * xnodp * (1.0 - eccsq) * (3.0 * cosio * cosio - 1.0) / (1.0 - satrec.ecco * satrec.ecco);

    if ((xnodp * constants.tumin) >= 0.001) {
        dsinit(opsmode, satrec.mdot, satrec.argpdot, satrec.nodedot, satrec, constants);
    }

    satrec.epoch = elem.epoch_jd;
    satrec.ds50 = elem.epoch_jd - 2433281.5;
}

} // namespace nadir::orbit