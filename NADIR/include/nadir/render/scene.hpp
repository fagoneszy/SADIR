#pragma once

#include <cstdint>
#include <vector>

#include <nadir/frames/reference_frame.hpp>
#include <nadir/render/scene_object.hpp>
#include <nadir/render/scene_primitive.hpp>
#include <nadir/state/reference_origin.hpp>
#include <nadir/time/instant.hpp>

namespace nadir::render {

struct SceneSnapshot {
    std::uint64_t sequence{};
    time::TimeInstant epoch{};
    frames::ReferenceFrame frame{};
    state::ReferenceOrigin origin{};
    std::vector<SceneObject> objects;
    std::vector<ScenePoint> points;
    std::vector<ScenePolyline> polylines;
    std::vector<SceneLabel> labels;

    [[nodiscard]] const SceneObject* find_object(std::uint64_t entity_id) const noexcept;
    [[nodiscard]] bool valid() const;
};

}
