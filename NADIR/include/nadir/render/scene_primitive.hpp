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
    // Physical radius in metres. The renderer draws a depth-aware uncertainty halo.
    double radius{1.0};
};

struct ScenePolyline {
    std::uint64_t entity_id{};
    std::vector<math::Vec3d> vertices;
    float intensity{1.0f};
    bool closed{};
};

struct SceneMeshEdge {
    std::size_t a{};
    std::size_t b{};
};

// A wireframe mesh attached to an existing scene object. Source provenance is
// carried into the snapshot so imported geometry remains traceable at render time.
struct SceneMesh {
    std::uint64_t entity_id{};
    std::vector<math::Vec3d> vertices;
    std::vector<SceneMeshEdge> edges;
    std::string source_path;
    std::string content_sha256;
    float intensity{1.0f};
};

struct SceneLabel {
    std::uint64_t entity_id{};
    math::Vec3d anchor{};
    std::string text;
    int priority{};
};

} // namespace nadir::render
