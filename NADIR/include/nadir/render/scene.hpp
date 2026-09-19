#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>

#include <nadir/math/vec3.hpp>

namespace nadir::render {

struct PointEntity {
    std::uint64_t id{};
    nadir::math::Vec3d position{};
    float intensity{1.0f};
    std::string label;
};

struct PolylineEntity {
    std::uint64_t id{};
    std::vector<nadir::math::Vec3d> points;
    float intensity{1.0f};
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
    std::vector<std::vector<nadir::math::Vec3d>> polylines;
    std::vector<LabelEntity> labels;
};

}