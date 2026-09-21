#pragma once

#include <span>
#include <string>
#include <vector>

#include <nadir/render/camera.hpp>
#include <nadir/render/depth_buffer.hpp>
#include <nadir/render/display_transform.hpp>
#include <nadir/render/ellipsoid_occlusion.hpp>
#include <nadir/render/phosphor_buffer.hpp>
#include <nadir/render/render_stats.hpp>
#include <nadir/render/scene.hpp>

namespace nadir::render {

struct ProjectedLabel {
    std::uint64_t entity_id{};
    int x{};
    int y{};
    double depth{};
    int priority{};
    std::string text;
};

class Renderer3D {
public:
    Renderer3D(int width, int height) : depth_(width, height), phosphor_(width, height) {}

    void resize(int width, int height) { depth_.resize(width, height); phosphor_.resize(width, height); }
    void set_ellipsoid_occlusion(EllipsoidOcclusion occlusion) noexcept { occlusion_ = occlusion; }
    [[nodiscard]] const EllipsoidOcclusion& ellipsoid_occlusion() const noexcept { return occlusion_; }

    bool render(const SceneSnapshot&, const Camera&, const DisplayTransform&, double delta_seconds);
    [[nodiscard]] const PhosphorBuffer& phosphor() const noexcept { return phosphor_; }
    [[nodiscard]] std::span<const ProjectedLabel> labels() const noexcept { return labels_; }
    [[nodiscard]] const RenderStats& stats() const noexcept { return stats_; }

private:
    DepthBuffer depth_;
    PhosphorBuffer phosphor_;
    std::vector<ProjectedLabel> labels_;
    RenderStats stats_{};
    EllipsoidOcclusion occlusion_{};
};

} // namespace nadir::render
