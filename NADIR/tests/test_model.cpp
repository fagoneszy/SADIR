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

void write_padded_chunk(std::ofstream& out, const std::string& bytes, std::uint32_t type) {
    const auto padded = (bytes.size() + 3U) & ~std::size_t{3U};
    write_u32_le(out, static_cast<std::uint32_t>(padded));
    write_u32_le(out, type);
    out.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    for (std::size_t i = bytes.size(); i < padded; ++i) out.put(type == 0x4e4f534aU ? ' ' : '\0');
}

} // namespace

int main() {
    const auto obj_path = std::filesystem::temp_directory_path() / "nadir-model-test.obj";
    const auto stl_path = std::filesystem::temp_directory_path() / "nadir-model-test.stl";
    const auto binary_stl_path = std::filesystem::temp_directory_path() / "nadir-model-test-binary.stl";
    const auto glb_path = std::filesystem::temp_directory_path() / "nadir-model-test.glb";
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
        const std::string document = R"({"asset":{"version":"2.0"},"buffers":[{"byteLength":42}],"bufferViews":[{"buffer":0,"byteOffset":0,"byteLength":36},{"buffer":0,"byteOffset":36,"byteLength":6}],"accessors":[{"bufferView":0,"componentType":5126,"count":3,"type":"VEC3"},{"bufferView":1,"componentType":5123,"count":3,"type":"SCALAR"}],"meshes":[{"primitives":[{"attributes":{"POSITION":0},"indices":1}]}]})";
        const auto json_size = (document.size() + 3U) & ~std::size_t{3U};
        constexpr std::size_t bin_size = 44;
        std::ofstream glb(glb_path, std::ios::binary);
        write_u32_le(glb, 0x46546c67U);
        write_u32_le(glb, 2);
        write_u32_le(glb, static_cast<std::uint32_t>(12U + 8U + json_size + 8U + bin_size));
        write_padded_chunk(glb, document, 0x4e4f534aU);
        std::string bin;
        for (const auto value : vertices) {
            const auto raw = std::bit_cast<std::uint32_t>(value);
            bin.push_back(static_cast<char>(raw)); bin.push_back(static_cast<char>(raw >> 8));
            bin.push_back(static_cast<char>(raw >> 16)); bin.push_back(static_cast<char>(raw >> 24));
        }
        bin.push_back(0); bin.push_back(0); bin.push_back(1); bin.push_back(0); bin.push_back(2); bin.push_back(0);
        write_padded_chunk(glb, bin, 0x004e4942U);
    }

    const auto obj = nadir::model::load_obj(obj_path);
    const auto stl = nadir::model::load_stl_ascii(stl_path);
    const auto binary_stl = nadir::model::load_stl(binary_stl_path);
    const auto glb = nadir::model::load_glb(glb_path);
    std::filesystem::remove(obj_path);
    std::filesystem::remove(stl_path);
    std::filesystem::remove(binary_stl_path);
    std::filesystem::remove(glb_path);
    if (obj.vertices.size() != 3 || obj.edges.size() != 3) return 1;
    if (stl.vertices.size() != 4 || stl.edges.size() != 5) return 2;
    if (binary_stl.vertices.size() != 3 || binary_stl.edges.size() != 3) return 3;
    if (glb.vertices.size() != 3 || glb.edges.size() != 3) return 4;
    if (obj.content_sha256.size() != 64 || stl.content_sha256.size() != 64 ||
        binary_stl.content_sha256.size() != 64 || glb.content_sha256.size() != 64) return 5;
    return nadir::model::load_stl_ascii("missing.stl").vertices.empty() ? 0 : 6;
}
