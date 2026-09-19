#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <nadir/math/vec3.hpp>

namespace nadir::render {

enum class PrimitiveKind : std::uint8_t { Point, Polyline, Mesh, Label };

struct ScenePoint {
    std::uint64_t entity_id{};
    math::Vec3d position{};
    float intensity{1.0f};
    double radius{1.0};
};

struct ScenePolyline {
    std::uint64_t entity_id{};
    std::vector<math::Vec3d> vertices;
    float intensity{1.0f};
    bool closed{};
};

struct SceneLabel {
    std::uint64_t entity_id{};
    math::Vec3d anchor{};
    std::string text;
    int priority{};
};

} // namespace nadir::render
