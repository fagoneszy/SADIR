#include <nadir/model/obj.hpp>

#include <filesystem>
#include <fstream>

int main() {
    const auto obj_path = std::filesystem::temp_directory_path() / "nadir-model-test.obj";
    const auto stl_path = std::filesystem::temp_directory_path() / "nadir-model-test.stl";
    {
        std::ofstream obj(obj_path);
        obj << "v 0 0 0\n"
               "v 1 0 0\n"
               "v 0 1 0\n"
               "f 1 2 3\n";
        std::ofstream stl(stl_path);
        stl << "solid square\n"
               "  facet normal 0 0 1\n"
               "    outer loop\n"
               "      vertex 0 0 0\n"
               "      vertex 1 0 0\n"
               "      vertex 1 1 0\n"
               "    endloop\n"
               "  endfacet\n"
               "  facet normal 0 0 1\n"
               "    outer loop\n"
               "      vertex 0 0 0\n"
               "      vertex 1 1 0\n"
               "      vertex 0 1 0\n"
               "    endloop\n"
               "  endfacet\n"
               "endsolid square\n";
    }

    const auto obj = nadir::model::load_obj(obj_path);
    const auto stl = nadir::model::load_stl_ascii(stl_path);
    std::filesystem::remove(obj_path);
    std::filesystem::remove(stl_path);
    if (obj.vertices.size() != 3 || obj.edges.size() != 3) return 1;
    if (stl.vertices.size() != 4 || stl.edges.size() != 5) return 2;
    return nadir::model::load_stl_ascii("missing.stl").vertices.empty() ? 0 : 3;
}
