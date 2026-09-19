#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>

#include <nadir/math/vec3.hpp>

namespace nadir::render {

enum class StateQuality {
    Unknown, Simulated, Observed, Estimated, Propagated, Predicted, Precise, Stale, Invalid
};

struct PointEntity {
    std::uint64_t id{};
    nadir::math::Vec3d position{};
    float intensity{1.0f};
    StateQuality quality{StateQuality::Unknown};
    std::string label;
};

struct PolylineEntity {
    std::uint64_t id{};
    std::vector<nadir::math::Vec3d> points;
    float intensity{1.0f};
    StateQuality quality{StateQuality::Unknown};
};

struct LabelEntity {
    std::uint64_t id{};
    nadir::math::Vec3d anchor{};
    std::string text;
};

struct SceneSnapshot {
    std::uint64_t sequence{};
    std::int64_t utc_ns{};

    std::vector<PointEntity> points;
    std::vector<PolylineEntity> polylines;
    std::vector<LabelEntity> labels;
};

}
