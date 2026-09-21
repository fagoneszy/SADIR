#include <nadir/render/scene_builder.hpp>

#include <cmath>
#include <limits>
#include <utility>

namespace nadir::render {

SceneBuilder::SceneBuilder(time::TimeInstant epoch, frames::ReferenceFrame frame,
                           state::ReferenceOrigin origin)
    : snapshot_{0, epoch, frame, origin} {}

bool SceneBuilder::add_object(const state::TrackedState& state) {
    if (!state.valid() || state.physical.epoch != snapshot_.epoch ||
        state.physical.frame != snapshot_.frame || state.physical.origin != snapshot_.origin ||
        next_entity_id_ == 0) return false;

    snapshot_.objects.push_back({next_entity_id_, {}, state});
    if (next_entity_id_ == std::numeric_limits<std::uint64_t>::max()) next_entity_id_ = 0;
    else ++next_entity_id_;
    return true;
}

void SceneBuilder::add_point(ScenePoint point) { snapshot_.points.push_back(std::move(point)); }
bool SceneBuilder::add_uncertainty_halo(std::uint64_t entity_id, double sigma_multiplier, float intensity) {
    if (!std::isfinite(sigma_multiplier) || sigma_multiplier <= 0.0 || !std::isfinite(intensity)) return false;
    const auto* object = snapshot_.find_object(entity_id);
    if (!object) return false;
    const auto sigma = state::position_sigma_m(object->state.uncertainty);
    if (!sigma || !std::isfinite(*sigma)) return false;
    const auto& position = object->state.physical.position.value;
    snapshot_.points.push_back({entity_id, {position.x, position.y, position.z}, intensity,
                                sigma_multiplier * *sigma});
    return true;
}
void SceneBuilder::add_polyline(ScenePolyline polyline) { snapshot_.polylines.push_back(std::move(polyline)); }
void SceneBuilder::add_label(SceneLabel label) { snapshot_.labels.push_back(std::move(label)); }
SceneSnapshot SceneBuilder::build() const { return snapshot_; }

} // namespace nadir::render
