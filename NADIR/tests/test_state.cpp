#include <nadir/state/tracked_state.hpp>

#include <type_traits>

static_assert(!std::is_convertible_v<nadir::units::PositionMeters, nadir::math::Vec3d>);

int main() {
    using namespace nadir;
    using namespace state;

    const PhysicalState teme{ {}, {}, {}, frames::teme(), earth_center() };
    const PhysicalState itrf{ {}, {}, {}, frames::itrf2020(), earth_center() };
    if (!teme.valid() || !itrf.valid()) return 1;

    const PhysicalState unknown_frame{ {}, {}, {}, {}, earth_center() };
    const PhysicalState no_origin{ {}, {}, {}, frames::teme(), {} };
    if (unknown_frame.valid() || no_origin.valid()) return 2;

    const PhysicalState enu_good{
        {}, {}, {}, frames::enu(42), {OriginKind::Observer, 42}
    };
    const PhysicalState enu_wrong{
        {}, {}, {}, frames::enu(42), {OriginKind::Observer, 99}
    };
    if (!enu_good.valid() || enu_wrong.valid()) return 3;

    const PhysicalState mars{
        {}, {}, {}, frames::body_fixed(499), body_center(499)
    };
    const PhysicalState moon_in_mars{
        {}, {}, {}, frames::body_fixed(499), body_center(301)
    };
    if (!mars.valid() || moon_in_mars.valid()) return 4;

    TrackedState tracked{1, teme, StateKind::Observed, StateQuality::Nominal};
    if (!tracked.valid()) return 5;
    tracked.kind = StateKind::Propagated;
    tracked.quality = StateQuality::Stale;
    if (!tracked.valid()) return 6;
    tracked.quality = StateQuality::Invalid;
    if (tracked.valid()) return 7;
    tracked.quality = StateQuality::Nominal;
    tracked.object_id = 0;
    if (tracked.valid()) return 8;

    const StateUncertainty unknown{};
    const StateUncertainty known{true, {}};
    if (unknown.known || !known.known) return 9;

    tracked.provenance = {7, 11, 13, 17, {19, 0}, {23, 0}, {}, true};
    if (tracked.provenance.source_id != 7 || tracked.provenance.model_id != 13 ||
        !tracked.provenance.source_verified) return 10;

    const Freshness freshness{{100, 0}, units::Seconds{10.0}};
    if (evaluate_freshness(freshness, {109, 999'999'999}) != FreshnessStatus::Fresh) return 11;
    if (evaluate_freshness(freshness, {110, 1}) != FreshnessStatus::Stale) return 12;
    if (evaluate_freshness(Freshness{}, {1, 0}) != FreshnessStatus::Unknown) return 13;

    return 0;
}
