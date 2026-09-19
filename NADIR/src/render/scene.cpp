#include <nadir/render/scene.hpp>

#include <cmath>
#include <unordered_set>

namespace nadir::render {

namespace {
bool finite(const math::Vec3d& value) noexcept {
    return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
}
} // namespace

const SceneObject* SceneSnapshot::find_object(std::uint64_t entity_id) const noexcept {
    for (const auto& object : objects) if (object.entity_id == entity_id) return &object;
    return nullptr;
}

bool SceneSnapshot::valid() const {
    if (!state::frame_origin_compatible(frame, origin)) return false;
    std::unordered_set<std::uint64_t> entity_ids;
    for (const auto& object : objects) {
        if (!object.valid() || object.state.physical.epoch != epoch ||
            object.state.physical.frame != frame || object.state.physical.origin != origin ||
            !entity_ids.insert(object.entity_id).second) return false;
    }
    for (const auto& point : points) {
        if (point.entity_id == 0 || !find_object(point.entity_id) || !finite(point.position) ||
            !std::isfinite(point.intensity) || !std::isfinite(point.radius)) return false;
    }
    for (const auto& polyline : polylines) {
        if (polyline.entity_id == 0 || !find_object(polyline.entity_id) || !std::isfinite(polyline.intensity)) return false;
        for (const auto& vertex : polyline.vertices) if (!finite(vertex)) return false;
    }
    for (const auto& label : labels) {
        if (label.entity_id == 0 || !find_object(label.entity_id) || !finite(label.anchor)) return false;
    }
    return true;
}

} // namespace nadir::render
