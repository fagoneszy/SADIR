#include <nadir/frames/orbit_adapter.hpp>
#include <nadir/geo/frames.hpp>

#include <cmath>

using namespace nadir;

static_assert(frames::teme().valid());
static_assert(frames::itrf2020().valid());
static_assert(!frames::ReferenceFrame{}.valid());
static_assert(frames::enu(42).valid());
static_assert(!frames::enu(0).valid());

int main() {
    using namespace frames;

    if (teme() != teme()) return 1;
    if (teme() == itrf2020()) return 2;
    if (enu(1) == enu(2)) return 3;
    if (body_fixed(499) == body_fixed(301)) return 4;

    const ReferenceFrame itrf2014{
        FrameKind::ITRF, {}, TerrestrialRealization::ITRF2014
    };
    if (itrf2014 == itrf2020()) return 5;

    if (orbit::to_reference_frame(orbit::Frame::TEME) != teme()) return 6;
    if (orbit::to_reference_frame(orbit::Frame::ITRF) != itrf2020()) return 7;
    if (orbit::to_reference_frame(orbit::Frame::Unknown).valid()) return 8;
    if (orbit::to_reference_frame(orbit::Frame::ENU).valid()) return 9;

    // Vallado 4th ed., GMST regression epoch (radians).
    if (std::abs(geo::gmst_vallado(2448855.009722) - 2.66300082) > 1.0e-7) return 10;

    // Vallado et al., AIAA 2006-6753 Rev. 2, Appendix C.  This is a
    // TEME->PEF->ITRF reference case, not a self-derived regression.
    const geo::StateVector teme{{5'094'180.16210, 6'127'644.65950, 6'380'344.53270},
                                {-4'746.131487, 785.818041, 5'531.931288}};
    const geo::EopRecord eop{.xp_arcsec = -0.140682, .yp_arcsec = 0.333309, .dut1_s = -0.439961};
    const auto itrf = geo::teme_to_itrf(teme, 2453101.8274118751, eop);
    const Vec3 expected_position{-1'033'479.38300, 7'901'295.27540, 6'380'356.59580};
    const Vec3 expected_velocity{-3'225.636520, -2'872.451450, 5'531.924446};
    if (length(itrf.position - expected_position) > 0.05 ||
        length(itrf.velocity - expected_velocity) > 0.001) return 11;

    return 0;
}
