#include <nadir/orbit/scene_adapter.hpp>

int main() {
    const nadir::time::TimeInstant epoch{42, 0};
    nadir::render::SceneBuilder builder{epoch, nadir::frames::itrf2020(), nadir::state::earth_center()};
    nadir::orbit::TrackingResult tracked{};
    tracked.itrf_m.position = {6'800'000.0, 0.0, 0.0}; tracked.itrf_m.velocity = {0.0, 7'600.0, 0.0};
    if (!nadir::orbit::add_tracking_scene_object(builder, 25544, tracked, epoch)) return 1;
    const auto scene = builder.build();
    if (!scene.valid() || scene.objects.size() != 1 || scene.objects.front().state.kind != nadir::state::StateKind::Propagated) return 2;
    return nadir::orbit::add_tracking_scene_object(builder, 0, tracked, epoch) ? 3 : 0;
}
