#include <nadir/render/renderer3d.hpp>
#include <nadir/render/scene_builder.hpp>

int main() {
    using namespace nadir;
    render::SceneBuilder builder{{1, 0}, frames::itrf2020(), state::earth_center()};
    state::TrackedState object{};
    object.object_id = 1;
    object.kind = state::StateKind::Propagated;
    object.quality = state::StateQuality::Nominal;
    object.physical = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1, 0}, frames::itrf2020(), state::earth_center()};
    if (!builder.add_object(object)) return 1;
    auto scene = builder.build();
    for (std::uint64_t i = 0; i < 12; ++i) scene.labels.push_back({1, {0.0, 0.0, 0.0}, "object", 0});
    scene.polylines.push_back({1, {{-1.0, 0.0, 0.0}, {-0.5, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.5, 0.0, 0.0}, {1.0, 0.0, 0.0}}, 1.0f, false});
    render::Renderer3D renderer(80, 40);
    renderer.set_lod({.max_labels = 3, .max_segments = 2, .minimum_detail = 1.0});
    render::Camera camera{};
    if (!renderer.render(scene, camera, {}, 0.0) || renderer.labels().size() != 3) return 2;
    if (renderer.stats().segments_submitted > 2) return 3;
    return 0;
}
