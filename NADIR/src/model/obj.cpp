#include <nadir/model/obj.hpp>
#include <fstream>
#include <set>
#include <sstream>
#include <string>

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

}
