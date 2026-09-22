#include <nadir/model/obj.hpp>
#include <cmath>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>

namespace nadir::model {

Mesh load_obj(const std::filesystem::path& path) {
    std::ifstream in(path);
    Mesh mesh;
    std::set<std::pair<std::size_t, std::size_t>> unique;
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream ss(line);
        std::string tag;
        ss >> tag;
        if (tag == "v") {
            Vec3 v{};
            ss >> v.x >> v.y >> v.z;
            mesh.vertices.push_back(v);
        } else if (tag == "f") {
            std::vector<std::size_t> face;
            std::string token;
            while (ss >> token) {
                const auto slash = token.find('/');
                const auto raw = token.substr(0, slash);
                const auto idx = static_cast<std::size_t>(std::stoull(raw));
                if (idx > 0) face.push_back(idx - 1);
            }
            if (face.size() >= 2) {
                for (std::size_t i = 0; i < face.size(); ++i) {
                    auto a = face[i];
                    auto b = face[(i + 1) % face.size()];
                    if (a > b) std::swap(a, b);
                    if (unique.emplace(a, b).second) mesh.edges.push_back({a, b});
                }
            }
        }
    }
    return mesh;
}

Mesh load_stl_ascii(const std::filesystem::path& path) {
    std::ifstream in(path);
    if (!in) return {};

    Mesh mesh;
    std::map<std::tuple<double, double, double>, std::size_t> vertex_indices;
    std::set<std::pair<std::size_t, std::size_t>> unique_edges;
    std::vector<std::size_t> triangle;
    bool in_loop = false;
    bool malformed = false;
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream ss(line);
        std::string tag;
        ss >> tag;
        if (tag == "outer") {
            std::string loop;
            if (in_loop || !(ss >> loop) || loop != "loop") {
                malformed = true;
                break;
            }
            triangle.clear();
            in_loop = true;
        } else if (tag == "vertex") {
            Vec3 vertex{};
            if (!in_loop || !(ss >> vertex.x >> vertex.y >> vertex.z) ||
                !std::isfinite(vertex.x) || !std::isfinite(vertex.y) || !std::isfinite(vertex.z) ||
                triangle.size() == 3) {
                malformed = true;
                break;
            }
            const auto key = std::tuple{vertex.x, vertex.y, vertex.z};
            const auto [it, inserted] = vertex_indices.emplace(key, mesh.vertices.size());
            if (inserted) mesh.vertices.push_back(vertex);
            triangle.push_back(it->second);
        } else if (tag == "endloop") {
            if (!in_loop || triangle.size() != 3) {
                malformed = true;
                break;
            }
            for (std::size_t i = 0; i < triangle.size(); ++i) {
                auto a = triangle[i];
                auto b = triangle[(i + 1) % triangle.size()];
                if (a > b) std::swap(a, b);
                if (a != b && unique_edges.emplace(a, b).second) mesh.edges.push_back({a, b});
            }
            in_loop = false;
        }
    }
    if (in_loop || malformed) return {};
    return mesh;
}

}
