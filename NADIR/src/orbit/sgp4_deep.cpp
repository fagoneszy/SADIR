#include <nadir/orbit/sgp4_internal.hpp>
#include <cmath>
#include <limits>

namespace nadir::orbit {

static constexpr double xj2 = 0.001082616;
static constexpr double xj3 = -0.00000253881;
static constexpr double xj4 = -0.00000165597;

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
) {
    sinim = std::sin(inclp);
    cosim = std::cos(inclp);
    emsq = ep * ep;
    snodm = std::sin(nodep);
    cnodm = std::cos(nodep);
    sini2 = sinim * sinim;
    cosi2 = cosim * cosim;
    emsq2 = emsq * emsq;
    eccm = ep;
    ms = 0.0;

    double zcosg = 0.91744867;
    double zsing = 0.39777719;
    double zcosi = 0.39777719;
    double zsini = 0.91744867;
    double zcosh = 0.91744867;
    double zsinh = -0.39777719;

    double zmo = 0.0;
    double zmp = 0.0;

    double q = std::abs(eccm);
    double qn = q;
    double qn2 = qn * qn;
    double qn3 = qn2 * qn;

    s1 = (1.0 + 3.0 * qn2 + 4.0 * qn3) / (1.0 - qn2) * (1.0 - qn2);
    s2 = (1.0 + 3.0 * qn2 + 4.0 * qn3) / (1.0 - qn2) * (1.0 - qn2);
    s3 = (1.0 + 3.0 * qn2 + 4.0 * qn3) / (1.0 - qn2) * (1.0 - qn2);
    s4 = (1.0 + 3.0 * qn2 + 4.0 * qn3) / (1.0 - qn2) * (1.0 - qn2);
    s5 = (1.0 + 3.0 * qn2 + 4.0 * qn3) / (1.0 - qn2) * (1.0 - qn2);
    s6 = (1.0 + 3.0 * qn2 + 4.0 * qn3) / (1.0 - qn2) * (1.0 - qn2);
    s7 = (1.0 + 3.0 * qn2 + 4.0 * qn3) / (1.0 - qn2) * (1.0 - qn2);

    ss1 = 0.0;
    ss2 = 0.0;
    ss3 = 0.0;
    ss4 = 0.0;
    ss5 = 0.0;
    ss6 = 0.0;
    ss7 = 0.0;

    sz1 = 0.0;
    sz2 = 0.0;
    sz3 = 0.0;
    sz11 = 0.0;
    sz12 = 0.0;
    sz13 = 0.0;
    sz21 = 0.0;
    sz22 = 0.0;
    sz23 = 0.0;
    sz31 = 0.0;
    sz32 = 0.0;
    sz33 = 0.0;

    zmol = 0.0;
    zmos = 0.0;
}

void dsinit(
    OpsMode opsmode,
    double& xmdot,
    double& omgdot,
    double& xnodot,
    SatRec& satrec,
    const Sgp4Constants& constants
) {
    satrec.deep_space = true;
    satrec.d2201 = 0.0;
    satrec.d2211 = 0.0;
    satrec.d3210 = 0.0;
    satrec.d3222 = 0.0;
    satrec.d4410 = 0.0;
    satrec.d4422 = 0.0;
    satrec.d5220 = 0.0;
    satrec.d5232 = 0.0;
    satrec.d5421 = 0.0;
    satrec.d5433 = 0.0;
    satrec.del1 = 0.0;
    satrec.del2 = 0.0;
    satrec.del3 = 0.0;
    satrec.fasx2 = 0.0;
    satrec.fasx4 = 0.0;
    satrec.fasx6 = 0.0;
    satrec.xlamo = 0.0;
    satrec.fact2 = 0.0;
    satrec.fact3 = 0.0;
    satrec.fact4 = 0.0;
    satrec.fact5 = 0.0;
    satrec.xfact = 0.0;
    satrec.xli = 0.0;
    satrec.xni = 0.0;
    satrec.atime = 0.0;
    satrec.xnodce = 0.0;
    satrec.xnodcf = 0.0;
    satrec.xmdot = xmdot;
    satrec.omgdot = omgdot;
    satrec.xnodot = xnodot;
    satrec.xll = 0.0;
    satrec.xnoh = 0.0;
    satrec.toth = 0.0;
    satrec.c1 = 0.0;
    satrec.c4 = 0.0;
    satrec.c5 = 0.0;
}

void dspace(
    double tsince,
    SatRec& satrec,
    const Sgp4Constants& constants
) {
    if (!satrec.deep_space) return;

    double t = tsince;
    double fact2 = 0.0;
    double fact3 = 0.0;
    double fact4 = 0.0;
    double fact5 = 0.0;
    double xfact = 0.0;
    double xli = 0.0;
    double xni = 0.0;
    double atime = 0.0;
    double xnodce = 0.0;
    double xnodcf = 0.0;

    satrec.xmdf = satrec.mdot * t;
    satrec.omgadf = satrec.omgdot * t;
    satrec.xnoddf = satrec.nodedot * t;
}

void dpper(
    SatRec& satrec,
    const Sgp4Constants& constants
) {
    if (!satrec.deep_space) return;
}

} // namespace nadir::orbit