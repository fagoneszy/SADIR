#pragma once
#include <nadir/core/math.hpp>
#include <filesystem>
#include <vector>

namespace nadir::model {

struct Edge {
    std::size_t a{};
    std::size_t b{};
};

struct Mesh {
    std::vector<Vec3> vertices;
    std::vector<Edge> edges;
};

Mesh load_obj(const std::filesystem::path& path);

// Loads the textual (ASCII) STL subset. Binary STL is intentionally not
// interpreted by this loader.
Mesh load_stl_ascii(const std::filesystem::path& path);

}
