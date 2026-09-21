#include <nadir/render/scene_builder.hpp>

#include <limits>

namespace {

nadir::state::TrackedState tracked(std::uint64_t object_id, nadir::time::TimeInstant epoch,
                                   nadir::frames::ReferenceFrame frame,
                                   nadir::state::ReferenceOrigin origin) {
    return {object_id, {{}, {}, epoch, frame, origin}, nadir::state::StateKind::Simulated,
            nadir::state::StateQuality::Nominal};
}

} // namespace

int main() {
    using namespace nadir;
    const time::TimeInstant epoch{100, 0};
    const auto frame = frames::itrf2020();
    const auto origin = state::earth_center();
    const auto object = tracked(25'544, epoch, frame, origin);

    render::SceneBuilder builder{epoch, frame, origin};
    if (!builder.add_object(object)) return 1;
    builder.add_point({1, {1.0, 2.0, 3.0}});
    builder.add_polyline({1, {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}}});
    builder.add_label({1, {1.0, 2.0, 3.0}, "ISS"});
    const auto snapshot = builder.build();
    if (!snapshot.valid() || !snapshot.find_object(1) || snapshot.find_object(2)) return 2;

    render::SceneBuilder frame_mismatch{epoch, frame, origin};
    if (frame_mismatch.add_object(tracked(1, epoch, frames::teme(), origin))) return 3;
    render::SceneBuilder epoch_mismatch{epoch, frame, origin};
    if (epoch_mismatch.add_object(tracked(1, {101, 0}, frame, origin))) return 4;
    render::SceneBuilder enu_mismatch{epoch, frame, origin};
    if (enu_mismatch.add_object(tracked(1, epoch, frames::enu(42), {state::OriginKind::Observer, 42}))) return 5;

    auto missing_primitive = snapshot;
    missing_primitive.points[0].entity_id = 99;
    if (missing_primitive.valid()) return 6;
    auto duplicate_id = snapshot;
    duplicate_id.objects.push_back(duplicate_id.objects.front());
    if (duplicate_id.valid()) return 7;
    auto nan_coordinate = snapshot;
    nan_coordinate.points[0].position.x = std::numeric_limits<double>::quiet_NaN();
    if (nan_coordinate.valid()) return 8;
    auto negative_radius = snapshot;
    negative_radius.points[0].radius = -1.0;
    if (negative_radius.valid()) return 9;
    auto uncertain = object;
    uncertain.uncertainty.known = true;
    uncertain.uncertainty.covariance.values[0] = 4.0;
    uncertain.uncertainty.covariance.values[7] = 4.0;
    uncertain.uncertainty.covariance.values[14] = 4.0;
    uncertain.uncertainty.covariance.values[21] = 1.0;
    uncertain.uncertainty.covariance.values[28] = 1.0;
    uncertain.uncertainty.covariance.values[35] = 1.0;
    render::SceneBuilder uncertainty_builder{epoch, frame, origin};
    if (!uncertainty_builder.add_object(uncertain) || !uncertainty_builder.add_uncertainty_halo(1)) return 10;
    const auto uncertainty_scene = uncertainty_builder.build();
    if (uncertainty_scene.points.size() != 1 || std::abs(uncertainty_scene.points[0].radius - 6.0) > 1e-12) return 11;
    if (uncertainty_builder.add_uncertainty_halo(99) || uncertainty_builder.add_uncertainty_halo(1, 0.0)) return 12;
    return 0;
}
