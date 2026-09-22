#include <nadir/model/obj.hpp>

#include <bit>
#include <cstdint>
#include <filesystem>
#include <fstream>

namespace {

void write_u32_le(std::ofstream& out, std::uint32_t value) {
    const char bytes[4]{static_cast<char>(value), static_cast<char>(value >> 8),
                        static_cast<char>(value >> 16), static_cast<char>(value >> 24)};
    out.write(bytes, sizeof(bytes));
}

void write_f32_le(std::ofstream& out, float value) {
    write_u32_le(out, std::bit_cast<std::uint32_t>(value));
}

} // namespace

int main() {
    const auto obj_path = std::filesystem::temp_directory_path() / "nadir-model-test.obj";
    const auto stl_path = std::filesystem::temp_directory_path() / "nadir-model-test.stl";
    const auto binary_stl_path = std::filesystem::temp_directory_path() / "nadir-model-test-binary.stl";
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
        std::ofstream binary(binary_stl_path, std::ios::binary);
        const char header[80]{};
        binary.write(header, sizeof(header));
        write_u32_le(binary, 1);
        for (int i = 0; i < 3; ++i) write_f32_le(binary, 0.0F);
        const float vertices[] = {0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F};
        for (const auto value : vertices) write_f32_le(binary, value);
        binary.put(0);
        binary.put(0);
    }

    const auto obj = nadir::model::load_obj(obj_path);
    const auto stl = nadir::model::load_stl_ascii(stl_path);
    const auto binary_stl = nadir::model::load_stl(binary_stl_path);
    std::filesystem::remove(obj_path);
    std::filesystem::remove(stl_path);
    std::filesystem::remove(binary_stl_path);
    if (obj.vertices.size() != 3 || obj.edges.size() != 3) return 1;
    if (stl.vertices.size() != 4 || stl.edges.size() != 5) return 2;
    if (binary_stl.vertices.size() != 3 || binary_stl.edges.size() != 3) return 3;
    return nadir::model::load_stl_ascii("missing.stl").vertices.empty() ? 0 : 4;
}
