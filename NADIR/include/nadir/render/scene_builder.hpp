#pragma once

#include <cstdint>

#include <nadir/render/scene.hpp>

namespace nadir::render {

class SceneBuilder {
public:
    explicit SceneBuilder(time::TimeInstant epoch, frames::ReferenceFrame frame,
                          state::ReferenceOrigin origin);

    bool add_object(const state::TrackedState& state);
    void add_point(ScenePoint point);
    void add_polyline(ScenePolyline polyline);
    void add_label(SceneLabel label);
    [[nodiscard]] SceneSnapshot build() const;

private:
    SceneSnapshot snapshot_;
    std::uint64_t next_entity_id_{1};
};

} // namespace nadir::render
