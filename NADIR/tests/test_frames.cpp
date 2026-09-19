#include <nadir/frames/orbit_adapter.hpp>

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

    return 0;
}
