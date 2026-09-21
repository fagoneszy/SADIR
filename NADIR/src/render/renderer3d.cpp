#include <nadir/render/renderer3d.hpp>

#include <nadir/render/clip.hpp>
#include <nadir/render/projection.hpp>

#include <algorithm>
#include <cmath>

namespace nadir::render {

bool Renderer3D::render(const SceneSnapshot& scene, const Camera& camera,
                        const DisplayTransform& transform, double delta_seconds) {
    stats_ = {};
    labels_.clear();
    depth_.clear();
    phosphor_.decay(std::max(0.0, delta_seconds));
    if (!scene.valid() || phosphor_.width() <= 0 || phosphor_.height() <= 0) return false;

    const double aspect = static_cast<double>(phosphor_.width()) / phosphor_.height();
    const math::Vec3d camera_m = transform.physical_origin_m +
        camera.position() * transform.meters_per_render_unit;
    const auto is_occluded = [&](std::uint64_t entity_id, const math::Vec3d& position_m) {
        return occlusion_.enabled && entity_id != occlusion_.occluder_entity_id &&
            occluded_by_ellipsoid(camera_m, position_m - occlusion_.center_m, occlusion_.ellipsoid);
    };
    const auto inside = [](const ProjectedPoint& point) {
        return point.visible && std::abs(point.x_ndc) <= 1.0 && std::abs(point.y_ndc) <= 1.0;
    };
    const double detail = std::clamp(4.0 / std::max(0.1, camera.distance), lod_.minimum_detail, 1.0);
    const std::size_t segment_budget = std::max<std::size_t>(1, static_cast<std::size_t>(lod_.max_segments * detail));
    const std::size_t label_budget = static_cast<std::size_t>(lod_.max_labels * detail);
    std::size_t total_segments{};
    for (const auto& line : scene.polylines)
        total_segments += line.vertices.size() > 1 ? line.vertices.size() - 1 + (line.closed ? 1 : 0) : 0;
    const std::size_t segment_stride = total_segments > segment_budget
        ? (total_segments + segment_budget - 1) / segment_budget : 1;
    const auto rasterize = [&](const NdcPoint& a, const NdcPoint& b, float intensity) {
        const auto first = ndc_to_viewport({a.x, a.y, 1.0 / a.inverse_depth, true},
                                           phosphor_.width(), phosphor_.height());
        const auto second = ndc_to_viewport({b.x, b.y, 1.0 / b.inverse_depth, true},
                                            phosphor_.width(), phosphor_.height());
        const int steps = std::max(1, static_cast<int>(std::ceil(std::max(
            std::abs(second.x - first.x), std::abs(second.y - first.y)))));
        for (int i = 0; i <= steps; ++i) {
            const double ratio = static_cast<double>(i) / steps;
            const double inverse_depth = a.inverse_depth + (b.inverse_depth - a.inverse_depth) * ratio;
            if (!std::isfinite(inverse_depth) || inverse_depth <= 0.0) continue;
            const int x = static_cast<int>(std::lround(first.x + (second.x - first.x) * ratio));
            const int y = static_cast<int>(std::lround(first.y + (second.y - first.y) * ratio));
            ++stats_.depth_tests;
            if (depth_.test_and_write(x, y, 1.0 / inverse_depth)) {
                ++stats_.depth_passes;
                phosphor_.inject(x, y, intensity);
            }
        }
    };
    const auto draw_segment = [&](std::uint64_t entity_id, const math::Vec3d& first_m,
                                  const math::Vec3d& second_m, float intensity) {
        if (is_occluded(entity_id, first_m) || is_occluded(entity_id, second_m)) return;
        const auto depth_segment = clip_depth(camera.world_to_view(to_render_space(first_m, transform)),
                                              camera.world_to_view(to_render_space(second_m, transform)),
                                              camera.near_plane, camera.far_plane);
        if (!depth_segment.visible) return;
        const auto first = project_perspective(depth_segment.a, camera.fov_deg, aspect,
                                               camera.near_plane, camera.far_plane);
        const auto second = project_perspective(depth_segment.b, camera.fov_deg, aspect,
                                                camera.near_plane, camera.far_plane);
        const auto ndc = clip_ndc({first.x_ndc, first.y_ndc, 1.0 / first.depth, first.visible},
                                  {second.x_ndc, second.y_ndc, 1.0 / second.depth, second.visible});
        if (!ndc.visible) return;
        rasterize(ndc.a, ndc.b, intensity);
        ++stats_.segments_visible;
        if (ndc.clipped) ++stats_.segments_clipped;
    };
    const auto draw_halo = [&](const ScreenPoint& center, double radius_m, float intensity) {
        if (radius_m <= 0.0 || transform.meters_per_render_unit <= 0.0) return;
        constexpr double pi = 3.14159265358979323846;
        const double focal_pixels = phosphor_.height() /
            (2.0 * std::tan(camera.fov_deg * pi / 360.0));
        const double radius_pixels = radius_m / transform.meters_per_render_unit * focal_pixels / center.depth;
        if (!std::isfinite(radius_pixels) || radius_pixels < 1.0) return;
        const int maximum_radius = 2 * std::max(phosphor_.width(), phosphor_.height());
        const int radius = std::clamp(static_cast<int>(std::lround(radius_pixels)), 1, maximum_radius);
        const int samples = std::clamp(static_cast<int>(std::ceil(2.0 * pi * radius)), 12, 4096);
        for (int sample = 0; sample < samples; ++sample) {
            const double angle = 2.0 * pi * sample / samples;
            const int x = static_cast<int>(std::lround(center.x + radius * std::cos(angle)));
            const int y = static_cast<int>(std::lround(center.y + radius * std::sin(angle)));
            ++stats_.depth_tests;
            if (center.depth <= depth_.get(x, y)) {
                ++stats_.depth_passes;
                phosphor_.inject(x, y, 0.35f * intensity);
            }
        }
    };

    for (const auto& point : scene.points) {
        ++stats_.points_submitted;
        if (is_occluded(point.entity_id, point.position)) continue;
        const auto projected = project_perspective(camera.world_to_view(to_render_space(point.position, transform)),
                                                   camera.fov_deg, aspect, camera.near_plane, camera.far_plane);
        if (!inside(projected)) continue;
        const auto screen = ndc_to_viewport(projected, phosphor_.width(), phosphor_.height());
        ++stats_.depth_tests;
        if (depth_.test_and_write(static_cast<int>(std::lround(screen.x)), static_cast<int>(std::lround(screen.y)), projected.depth)) {
            phosphor_.inject(static_cast<int>(std::lround(screen.x)), static_cast<int>(std::lround(screen.y)), point.intensity);
            draw_halo(screen, point.radius, point.intensity);
            ++stats_.depth_passes;
            ++stats_.points_visible;
        }
    }
    for (const auto& line : scene.polylines) {
        for (std::size_t i = segment_stride; i < line.vertices.size(); i += segment_stride) {
            ++stats_.segments_submitted;
            draw_segment(line.entity_id, line.vertices[i - segment_stride], line.vertices[i], line.intensity);
        }
        if (line.vertices.size() > 1 && (line.vertices.size() - 1) % segment_stride != 0) {
            ++stats_.segments_submitted;
            draw_segment(line.entity_id, line.vertices[((line.vertices.size() - 1) / segment_stride) * segment_stride], line.vertices.back(), line.intensity);
        }
        if (line.closed && line.vertices.size() > 2) {
            ++stats_.segments_submitted;
            draw_segment(line.entity_id, line.vertices.back(), line.vertices.front(), line.intensity);
        }
    }
    for (const auto& label : scene.labels) {
        if (label.text.empty() || is_occluded(label.entity_id, label.anchor)) continue;
        const auto projected = project_perspective(camera.world_to_view(to_render_space(label.anchor, transform)),
                                                   camera.fov_deg, aspect, camera.near_plane, camera.far_plane);
        if (!inside(projected)) continue;
        const auto screen = ndc_to_viewport(projected, phosphor_.width(), phosphor_.height());
        labels_.push_back({label.entity_id, static_cast<int>(std::lround(screen.x)),
                           static_cast<int>(std::lround(screen.y)), projected.depth,
                           label.priority, label.text});
    }
    std::sort(labels_.begin(), labels_.end(), [](const auto& a, const auto& b) {
        return a.priority != b.priority ? a.priority > b.priority : a.depth < b.depth;
    });
    if (labels_.size() > label_budget) labels_.resize(label_budget);
    return true;
}

} // namespace nadir::render
