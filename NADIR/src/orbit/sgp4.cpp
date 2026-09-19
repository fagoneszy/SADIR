#include <nadir/orbit/sgp4.hpp>
#include <nadir/orbit/sgp4_internal.hpp>
#include <cmath>
#include <limits>
#include <cstdio>

namespace nadir::orbit {

Sgp4Elements omm_to_elements(const astro::OmmRecord& rec) {
    Sgp4Elements elem;
    elem.norad = rec.norad_cat_id;
    elem.bstar = rec.bstar;
    elem.eccentricity = rec.eccentricity;
    elem.inclination_rad = deg_to_rad(rec.inclination_deg);
    elem.raan_rad = deg_to_rad(rec.raan_deg);
    elem.arg_perigee_rad = deg_to_rad(rec.arg_pericenter_deg);
    elem.mean_anomaly_rad = deg_to_rad(rec.mean_anomaly_deg);
    elem.mean_motion_rad_min = rev_day_to_rad_min(rec.mean_motion_rev_day);

    elem.epoch_jd = 0.0;
    if (!rec.epoch.empty()) {
        int year = 0, month = 0, day = 0, hour = 0, minute = 0;
        double second = 0.0;
        if (std::sscanf(rec.epoch.c_str(), "%d-%d-%dT%d:%d:%lf", &year, &month, &day, &hour, &minute, &second) == 6) {
            double a = (14.0 - month) / 12.0;
            double y = year + 4800.0 - a;
            double m = month + 12.0 * a - 3.0;
            double jd = day + std::floor((153.0 * m + 2.0) / 5.0) + std::floor(365.0 * y) + std::floor(y / 4.0) - std::floor(y / 100.0) + std::floor(y / 400.0) - 32045.0;
            jd += (hour + minute / 60.0 + second / 3600.0) / 24.0;
            elem.epoch_jd = jd;
        }
    }
    return elem;
}

std::string to_string(Sgp4Error error) {
    switch (error) {
        case Sgp4Error::None: return "None";
        case Sgp4Error::MeanEccentricity: return "MeanEccentricity";
        case Sgp4Error::MeanMotion: return "MeanMotion";
        case Sgp4Error::PerturbedEccentricity: return "PerturbedEccentricity";
        case Sgp4Error::SemiLatusRectum: return "SemiLatusRectum";
        case Sgp4Error::Decayed: return "Decayed";
        case Sgp4Error::InvalidElements: return "InvalidElements";
        case Sgp4Error::NumericalFailure: return "NumericalFailure";
        default: return "Unknown";
    }
}

Sgp4Result propagate_sgp4(
    const astro::OmmRecord& elements,
    double minutes_since_epoch
) {
    const auto constants = wgs72_constants();
    const auto elem = omm_to_elements(elements);

    SatRec satrec{};
    sgp4init(OpsMode::Afspc, elem, satrec, constants);

    if (!std::isfinite(satrec.no_kozai)) {
        return Sgp4Result{
            State{{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0, 0.0, 0.0, 0.0}, Frame::TEME},
            Sgp4Error::InvalidElements
        };
    }

    Vec3d position_km{};
    Vec3d velocity_km_s{};

    sgp4(satrec, minutes_since_epoch, position_km, velocity_km_s, constants);

    if (!std::isfinite(position_km.x) || !std::isfinite(position_km.y) || !std::isfinite(position_km.z) ||
        !std::isfinite(velocity_km_s.x) || !std::isfinite(velocity_km_s.y) || !std::isfinite(velocity_km_s.z)) {
        return Sgp4Result{
            State{{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0, 0.0, 0.0, 0.0}, Frame::TEME},
            Sgp4Error::NumericalFailure
        };
    }

    return Sgp4Result{
        State{
            position_km,
            velocity_km_s,
            {0, elem.epoch_jd, 0.0, 0.0},
            Frame::TEME
        },
        Sgp4Error::None
    };
}

namespace {

void sgp4_ne(
    SatRec& satrec,
    double tsince_min,
    Vec3d& position_km,
    Vec3d& velocity_km_s,
    const Sgp4Constants& constants
) {
    const double two_pi = 2.0 * pi;
    const double xke = constants.xke;
    const double j2 = constants.j2;
    const double j3 = constants.j3;
    const double j4 = constants.j4;
    const double j3oj2 = constants.j3oj2;
    const double tumin = constants.tumin;

    double xmdf = satrec.mo + satrec.mdot * tsince_min;
    double argpdf = satrec.argpo + satrec.argpdot * tsince_min;
    double nodedf = satrec.nodeo + satrec.nodedot * tsince_min;
    double argpm = argpdf;
    double mm = xmdf;

    double t2 = tsince_min * tsince_min;

    double nodem = nodedf + satrec.nodecf * t2;

    double tempa = 1.0 - satrec.cc1 * tsince_min;
    double tempe = satrec.bstar * satrec.cc4 * tsince_min;
    double templ = satrec.t2cof * t2;

    if (!satrec.simplified) {
        double delomg = satrec.omgcof * tsince_min;
        double delm = satrec.xmcof *
            (std::pow(1.0 + satrec.eta * std::cos(xmdf), 3.0) - satrec.delmo);
        double temp = delomg + delm;
        mm += temp;
        argpm -= temp;

        double t3 = t2 * tsince_min;
        double t4 = t3 * tsince_min;

        tempa -= satrec.d2 * t2 + satrec.d3 * t3 + satrec.d4 * t4;
        tempe += satrec.bstar * satrec.cc5 * (std::sin(mm) - satrec.sinmao);
        templ += satrec.t3cof * t3 + t4 * (satrec.t4cof + tsince_min * satrec.t5cof);
    }

    double a = std::pow(constants.xke / satrec.no_kozai, 2.0 / 3.0) * tempa * tempa;
    a = std::max(a, 1.0);
    double n = constants.xke / std::pow(a, 1.5);
    double e = satrec.ecco - tempe;

    if (e < 0.0 || e > 1.0 || a * (1.0 - e) < 1.0 + constants.j2) {
        return;
    }

    double pl = a * (1.0 - e * e);
    double r = pl / (1.0 + e * std::cos(mm));
    double u = mm + argpm;

    double su = std::sin(u);
    double cu = std::cos(u);

    double sin2u = std::sin(2.0 * u);
    double cos2u = std::cos(2.0 * u);

    double sin_omega = std::sin(satrec.omega);
    double cos_omega = std::cos(satrec.omega);
    double sin_nodedf = std::sin(nodedf);
    double cos_nodedf = std::cos(nodedf);
    double sin_inc = std::sin(satrec.inclo);
    double cos_inc = std::cos(satrec.inclo);

    double rdot = e * n * a * std::sin(mm) / std::sqrt(1.0 - e * e);
    double rv = std::sqrt(a * (1.0 - e * e)) / r;

    double x = r * cu;
    double y = r * su * cos_inc;
    double z = r * su * sin_inc;

    double vx = rdot * cu - rv * su * cos_inc;
    double vy = rdot * su + rv * cu * cos_inc;
    double vz = rv * su * sin_inc;

    double cosnode = std::cos(nodem);
    double sinnode = std::sin(nodem);

    double x_km = x * cosnode - y * sinnode;
    double y_km = x * sinnode + y * cosnode;
    double z_km = z;

    double vx_km_s = vx * cosnode - vy * sinnode;
    double vy_km_s = vx * sinnode + vy * cosnode;
    double vz_km_s = vz;

    position_km = {x_km, y_km, z_km};
    velocity_km_s = {vx_km_s, vy_km_s, vz_km_s};
}

void sgp4_ds(
    SatRec& satrec,
    double tsince_min,
    Vec3d& position_km,
    Vec3d& velocity_km_s,
    const Sgp4Constants& constants
) {
    sgp4_ne(satrec, tsince_min, position_km, velocity_km_s, constants);
}

}

void sgp4(
    SatRec& satrec,
    double tsince,
    Vec3d& position_km,
    Vec3d& velocity_km_s,
    const Sgp4Constants& constants
) {
    if (satrec.simplified || satrec.no_kozai <= 0.0) {
        position_km = {0.0, 0.0, 0.0};
        velocity_km_s = {0.0, 0.0, 0.0};
        return;
    }

    const double tsince_min = tsince * constants.tumin;

    if (satrec.deep_space) {
        dspace(tsince_min, satrec, constants);
        sgp4_ne(satrec, tsince_min, position_km, velocity_km_s, constants);
    } else {
        sgp4_ne(satrec, tsince_min, position_km, velocity_km_s, constants);
    }
}

} // namespace nadir::orbit