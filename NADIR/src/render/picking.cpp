#include <nadir/render/picking.hpp>

#include <nadir/render/projection.hpp>

#include <cmath>

namespace nadir::render {
std::optional<PickResult> pick_scene(const SceneSnapshot& scene, const Camera& camera,
                                     const DisplayTransform& transform, int width, int height,
                                     double x, double y, double radius) {
    if (!scene.valid() || width <= 0 || height <= 0 || !std::isfinite(x) || !std::isfinite(y) || radius < 0.0) return std::nullopt;
    std::optional<PickResult> best;
    const double aspect = static_cast<double>(width) / height;
    for (const auto& point : scene.points) {
        const auto projected = project_perspective(camera.world_to_view(to_render_space(point.position, transform)),
                                                   camera.fov_deg, aspect, camera.near_plane, camera.far_plane);
        const auto screen = ndc_to_viewport(projected, width, height);
        if (!screen.visible) continue;
        const double distance = std::hypot(screen.x - x, screen.y - y);
        if (distance > radius) continue;
        if (!best || distance < best->distance_pixels ||
            (distance == best->distance_pixels && point.entity_id < best->entity_id))
            best = {point.entity_id, distance, screen.depth};
    }
    return best;
}
} // namespace nadir::render
