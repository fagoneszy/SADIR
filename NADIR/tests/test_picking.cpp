#include <nadir/render/picking.hpp>
#include <nadir/render/scene_builder.hpp>

int main() {
    using namespace nadir;
    const auto epoch = time::TimeInstant{}; const auto frame = frames::itrf2020(); const auto origin = state::earth_center();
    render::SceneBuilder builder{epoch, frame, origin};
    state::TrackedState one{1, {{}, {}, epoch, frame, origin}, state::StateKind::Simulated, state::StateQuality::Nominal};
    state::TrackedState two{2, {{}, {}, epoch, frame, origin}, state::StateKind::Simulated, state::StateQuality::Nominal};
    if (!builder.add_object(one) || !builder.add_object(two)) return 1;
    builder.add_point({1, {0.0, 0.0, -10.0}}); builder.add_point({2, {0.0, 0.0, -10.0}});
    render::Camera camera{}; camera.distance = 0.0; camera.near_plane = 0.1; camera.far_plane = 100.0;
    const auto hit = render::pick_scene(builder.build(), camera, { {}, 1.0 }, 101, 101, 50.0, 50.0);
    if (!hit || hit->entity_id != 1) return 2;
    return render::pick_scene(builder.build(), camera, { {}, 1.0 }, 101, 101, 0.0, 0.0) ? 3 : 0;
}
