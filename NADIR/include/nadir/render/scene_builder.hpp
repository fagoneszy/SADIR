#pragma once

#include <cstdint>

#include <nadir/render/scene.hpp>
#include <nadir/model/obj.hpp>

namespace nadir::render {

class SceneBuilder {
public:
    explicit SceneBuilder(time::TimeInstant epoch, frames::ReferenceFrame frame,
                          state::ReferenceOrigin origin);

    bool add_object(const state::TrackedState& state);
    void add_point(ScenePoint point);
    // Adds a physical-position uncertainty halo for an object with valid covariance.
    bool add_uncertainty_halo(std::uint64_t entity_id, double sigma_multiplier = 3.0,
                              float intensity = 1.0f);
    void add_polyline(ScenePolyline polyline);
    bool add_mesh(std::uint64_t entity_id, const model::Mesh& mesh, float intensity = 1.0f);
    void add_label(SceneLabel label);
    [[nodiscard]] SceneSnapshot build() const;

private:
    SceneSnapshot snapshot_;
    std::uint64_t next_entity_id_{1};
};

} // namespace nadir::render
