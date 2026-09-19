#include <nadir/render/scene_builder.hpp>

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
void SceneBuilder::add_polyline(ScenePolyline polyline) { snapshot_.polylines.push_back(std::move(polyline)); }
void SceneBuilder::add_label(SceneLabel label) { snapshot_.labels.push_back(std::move(label)); }
SceneSnapshot SceneBuilder::build() const { return snapshot_; }

} // namespace nadir::render
