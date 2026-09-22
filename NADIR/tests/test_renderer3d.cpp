#include <nadir/render/clip.hpp>
#include <nadir/render/depth_buffer.hpp>
#include <nadir/render/renderer3d.hpp>
#include <nadir/render/scene_builder.hpp>
#include <nadir/model/obj.hpp>

#include <cmath>
#include <limits>

int main() {
    using namespace nadir;

    render::DepthBuffer depth(2, 2);
    if (!depth.test_and_write(0, 0, 5.0) || depth.test_and_write(0, 0, 8.0) ||
        !depth.test_and_write(0, 0, 2.0) || depth.get(0, 0) != 2.0) return 1;
    if (depth.test_and_write(0, 0, 0.0) || depth.test_and_write(-1, 0, 1.0) ||
        depth.test_and_write(0, 0, std::numeric_limits<double>::quiet_NaN())) return 2;
    depth.clear();
    if (depth.get(0, 0) != std::numeric_limits<double>::infinity()) return 3;

    const auto near_clip = render::clip_depth({0.0, 0.0, -0.5}, {0.0, 0.0, -4.0}, 1.0, 3.0);
    if (!near_clip.visible || std::abs(near_clip.a.z + 1.0) > 1e-12 ||
        std::abs(near_clip.b.z + 3.0) > 1e-12) return 4;
    if (render::clip_depth({0.0, 0.0, -0.2}, {0.0, 0.0, -0.4}, 1.0, 3.0).visible) return 5;

    const auto ndc_clip = render::clip_ndc({-2.0, 0.0, 1.0, true}, {0.5, 0.0, 0.25, true});
    if (!ndc_clip.visible || !ndc_clip.clipped || std::abs(ndc_clip.a.x + 1.0) > 1e-12 ||
        std::abs(ndc_clip.a.inverse_depth - 0.7) > 1e-12) return 6;
    if (render::clip_ndc({2.0, 2.0, 1.0, true}, {3.0, 3.0, 1.0, true}).visible) return 7;
    if (render::clip_ndc({0.0, 0.0, std::numeric_limits<double>::quiet_NaN(), true},
                         {0.5, 0.0, 1.0, true}).visible) return 8;
    const time::TimeInstant epoch{0, 0};
    const auto frame = frames::itrf2020();
    const auto origin = state::earth_center();
    render::SceneBuilder builder{epoch, frame, origin};
    if (!builder.add_object({1, {{}, {}, epoch, frame, origin}, state::StateKind::Simulated,
                             state::StateQuality::Nominal})) return 9;
    builder.add_point({1, {}, 1.0f, 1.0});
    render::Renderer3D renderer{100, 100};
    render::Camera camera{};
    if (!renderer.render(builder.build(), camera, {{}, 1.0}, 0.0)) return 10;
    int illuminated{};
    for (int y = 0; y < 100; ++y) for (int x = 0; x < 100; ++x)
        if (renderer.phosphor().get(x, y) > 0.0f) ++illuminated;
    if (illuminated < 40 || renderer.stats().points_visible != 1) return 11;
    render::SceneBuilder mesh_builder{epoch, frame, origin};
    if (!mesh_builder.add_object({1, {{}, {}, epoch, frame, origin}, state::StateKind::Simulated,
                                  state::StateQuality::Nominal})) return 12;
    const model::Mesh mesh{{{-1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}}, {{0, 1}}, "fixture.obj", "hash"};
    if (!mesh_builder.add_mesh(1, mesh) || !renderer.render(mesh_builder.build(), camera, {{}, 1.0}, 0.0) ||
        renderer.stats().segments_submitted != 1 || renderer.stats().segments_visible != 1) return 13;
    return 0;
}
