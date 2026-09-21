#include <nadir/core/terminal.hpp>
#include <nadir/render/frame_clock.hpp>
#include <nadir/render/presenter.hpp>
#include <nadir/render/renderer3d.hpp>
#include <nadir/render/scene_builder.hpp>

namespace {

nadir::state::TrackedState demo_object(nadir::time::TimeInstant epoch,
                                       nadir::frames::ReferenceFrame frame,
                                       nadir::state::ReferenceOrigin origin) {
    return {1, {{}, {}, epoch, frame, origin}, nadir::state::StateKind::Simulated,
            nadir::state::StateQuality::Nominal};
}

} // namespace

int main() {
    using namespace nadir;
    TerminalSession terminal;
    if (!terminal.valid()) return 1;

    const auto epoch = time::TimeInstant{0, 0};
    const auto frame = frames::itrf2020();
    const auto origin = state::earth_center();
    render::SceneBuilder builder{epoch, frame, origin};
    if (!builder.add_object(demo_object(epoch, frame, origin))) return 2;

    constexpr double s = 2.0;
    const math::Vec3d v[] = {{-s, -s, -s}, {s, -s, -s}, {s, s, -s}, {-s, s, -s},
                             {-s, -s, s},  {s, -s, s},  {s, s, s},  {-s, s, s}};
    constexpr int edges[][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6},
                                {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};
    for (const auto& edge : edges) builder.add_polyline({1, {v[edge[0]], v[edge[1]]}, 1.0f});
    builder.add_label({1, {0.0, 0.0, 0.0}, "NADIR RENDER DEMO", 10});
    const auto scene = builder.build();

    auto size = terminal.size();
    render::Renderer3D renderer{size.columns * 2, size.rows * 4};
    render::Presenter presenter{size.columns, size.rows};
    render::Camera camera{};
    camera.distance = 8.0;
    camera.near_plane = 0.1;
    camera.far_plane = 100.0;
    render::FrameClock clock{30.0};
    std::uint64_t frame_number = 0;

    while (true) {
        const auto input = terminal.poll_input();
        if (input.quit) break;
        if (input.left) camera.yaw(-0.05);
        if (input.right) camera.yaw(0.05);
        if (input.up) camera.pitch(0.05);
        if (input.down) camera.pitch(-0.05);
        if (input.zoom_in) camera.zoom(-0.2);
        if (input.zoom_out) camera.zoom(0.2);
        camera.yaw(0.25 * clock.tick());

        size = terminal.size();
        renderer.resize(size.columns * 2, size.rows * 4);
        presenter.resize(size.columns, size.rows);
        renderer.render(scene, camera, {{}, 1.0}, 0.0);
        presenter.render(renderer.phosphor(), {30.0, static_cast<int>(frame_number++), "DEMO",
                                               "ITRF2020", "ORBIT", false, false, false, true, 1});
        presenter.present(terminal);
        clock.wait();
    }
    return 0;
}
