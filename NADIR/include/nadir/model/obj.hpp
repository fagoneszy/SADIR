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
    std::string source_path;
    std::string content_sha256;
};

Mesh load_obj(const std::filesystem::path& path);

// Loads the textual (ASCII) STL subset.
Mesh load_stl_ascii(const std::filesystem::path& path);

// Loads either ASCII STL or the standard little-endian binary STL encoding.
Mesh load_stl(const std::filesystem::path& path);

// Loads the first TRIANGLES primitive in a GLB 2.0 file with an embedded BIN
// chunk. POSITION must be a FLOAT VEC3 accessor; indexed UINT16/UINT32 input
// is supported.
Mesh load_glb(const std::filesystem::path& path);

}
