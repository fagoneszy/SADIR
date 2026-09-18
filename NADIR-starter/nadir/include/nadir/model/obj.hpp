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

}
