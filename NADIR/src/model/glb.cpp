#include <nadir/model/obj.hpp>
#include <nadir/core/json.hpp>
#include <nadir/core/sha256.hpp>

#include <bit>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

namespace nadir::model {
namespace {
constexpr std::uint32_t glb_magic = 0x46546c67U;
constexpr std::uint32_t json_chunk = 0x4e4f534aU;
constexpr std::uint32_t bin_chunk = 0x004e4942U;
constexpr std::size_t max_glb_bytes = 64U * 1024U * 1024U;

std::uint32_t u32(const char* bytes) {
    return static_cast<std::uint32_t>(static_cast<unsigned char>(bytes[0])) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(bytes[1])) << 8) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(bytes[2])) << 16) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(bytes[3])) << 24);
}
double f32(const char* bytes) { return std::bit_cast<float>(u32(bytes)); }

std::optional<std::size_t> size_value(const json::Value* value) {
    if (!value || !value->is_number()) return std::nullopt;
    const auto number = value->as_number();
    if (!std::isfinite(number) || number < 0.0 || std::floor(number) != number ||
        number > static_cast<double>(std::numeric_limits<std::size_t>::max())) return std::nullopt;
    return static_cast<std::size_t>(number);
}

const json::Object* object_at(const json::Array* array, std::size_t index) {
    return array && index < array->size() ? (*array)[index].as_object() : nullptr;
}

const json::Value* field(const json::Object* object, std::string_view key) {
    if (!object) return nullptr;
    const auto it = object->find(key);
    return it == object->end() ? nullptr : &it->second;
}

bool range_in(std::size_t offset, std::size_t bytes, std::size_t size) {
    return offset <= size && bytes <= size - offset;
}

bool append_triangle(Mesh& mesh, std::map<std::tuple<double, double, double>, std::size_t>& vertices,
                     std::set<std::pair<std::size_t, std::size_t>>& edges, const Vec3 (&triangle)[3]) {
    std::size_t indices[3]{};
    for (std::size_t i = 0; i < 3; ++i) {
        if (!std::isfinite(triangle[i].x) || !std::isfinite(triangle[i].y) || !std::isfinite(triangle[i].z)) return false;
        const auto [it, inserted] = vertices.emplace(std::tuple{triangle[i].x, triangle[i].y, triangle[i].z}, mesh.vertices.size());
        if (inserted) mesh.vertices.push_back(triangle[i]);
        indices[i] = it->second;
    }
    for (std::size_t i = 0; i < 3; ++i) {
        auto a = indices[i];
        auto b = indices[(i + 1) % 3];
        if (a > b) std::swap(a, b);
        if (a != b && edges.emplace(a, b).second) mesh.edges.push_back({a, b});
    }
    return true;
}
} // namespace

Mesh load_glb(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary | std::ios::ate);
    if (!input) return {};
    const auto end = input.tellg();
    if (end < 20 || static_cast<unsigned long long>(end) > max_glb_bytes) return {};
    std::vector<char> bytes(static_cast<std::size_t>(end));
    input.seekg(0);
    if (!input.read(bytes.data(), static_cast<std::streamsize>(bytes.size())) || u32(bytes.data()) != glb_magic ||
        u32(bytes.data() + 4) != 2 || u32(bytes.data() + 8) != bytes.size()) return {};
    std::size_t cursor = 12;
    std::string json_text;
    std::vector<char> bin;
    while (range_in(cursor, 8, bytes.size())) {
        const auto length = static_cast<std::size_t>(u32(bytes.data() + cursor));
        const auto type = u32(bytes.data() + cursor + 4);
        cursor += 8;
        if (!range_in(cursor, length, bytes.size())) return {};
        if (type == json_chunk && json_text.empty()) json_text.assign(bytes.data() + cursor, length);
        if (type == bin_chunk && bin.empty()) bin.assign(bytes.begin() + static_cast<std::ptrdiff_t>(cursor),
                                                          bytes.begin() + static_cast<std::ptrdiff_t>(cursor + length));
        cursor += length;
    }
    while (!json_text.empty() && (json_text.back() == ' ' || json_text.back() == '\t' || json_text.back() == '\r' ||
                                  json_text.back() == '\n' || json_text.back() == '\0')) json_text.pop_back();
    const auto parsed = json::parse(json_text);
    const auto* root = parsed.ok ? parsed.value.as_object() : nullptr;
    const auto* views = root && root->contains("bufferViews") ? root->at("bufferViews").as_array() : nullptr;
    const auto* accessors = root && root->contains("accessors") ? root->at("accessors").as_array() : nullptr;
    const auto* meshes = root && root->contains("meshes") ? root->at("meshes").as_array() : nullptr;
    const auto* mesh = object_at(meshes, 0);
    const auto* primitives = mesh && mesh->contains("primitives") ? mesh->at("primitives").as_array() : nullptr;
    const auto* primitive = object_at(primitives, 0);
    const auto* attributes = primitive && primitive->contains("attributes") ? primitive->at("attributes").as_object() : nullptr;
    if (!primitive || !attributes || !attributes->contains("POSITION")) return {};
    const auto position_accessor_index = size_value(&attributes->at("POSITION"));
    const auto* position_accessor = position_accessor_index ? object_at(accessors, *position_accessor_index) : nullptr;
    if (!position_accessor || position_accessor->at("componentType").as_number() != 5126.0 ||
        position_accessor->at("type").as_string() != "VEC3") return {};
    const auto position_view_index = size_value(field(position_accessor, "bufferView"));
    const auto position_count = size_value(field(position_accessor, "count"));
    const auto* position_view = position_view_index ? object_at(views, *position_view_index) : nullptr;
    if (!position_view || !position_count) return {};
    const auto view_offset = size_value(field(position_view, "byteOffset")).value_or(0);
    const auto accessor_offset = size_value(field(position_accessor, "byteOffset")).value_or(0);
    const auto stride = size_value(field(position_view, "byteStride")).value_or(12);
    if (stride < 12 || !range_in(view_offset, accessor_offset, bin.size()) ||
        (*position_count && !range_in(view_offset + accessor_offset, 12 + (*position_count - 1) * stride, bin.size()))) return {};
    const auto vertex = [&](std::size_t index) -> std::optional<Vec3> {
        if (index >= *position_count) return std::nullopt;
        const auto offset = view_offset + accessor_offset + index * stride;
        return Vec3{f32(bin.data() + offset), f32(bin.data() + offset + 4), f32(bin.data() + offset + 8)};
    };
    std::vector<std::size_t> indices;
    if (primitive->contains("indices")) {
        const auto accessor_index = size_value(&primitive->at("indices"));
        const auto* accessor = accessor_index ? object_at(accessors, *accessor_index) : nullptr;
        const auto view_index = accessor ? size_value(field(accessor, "bufferView")) : std::nullopt;
        const auto count = accessor ? size_value(field(accessor, "count")) : std::nullopt;
        const auto* view = view_index ? object_at(views, *view_index) : nullptr;
        const auto component = accessor ? size_value(field(accessor, "componentType")) : std::nullopt;
        if (!accessor || !view || !count || !component || (*component != 5123 && *component != 5125)) return {};
        const auto element_size = *component == 5123 ? 2U : 4U;
        const auto offset = size_value(field(view, "byteOffset")).value_or(0) + size_value(field(accessor, "byteOffset")).value_or(0);
        if (*count % 3 != 0 || (*count && !range_in(offset, *count * element_size, bin.size()))) return {};
        for (std::size_t i = 0; i < *count; ++i) {
            const auto at = bin.data() + offset + i * element_size;
            indices.push_back(element_size == 2 ? static_cast<unsigned char>(at[0]) | (static_cast<std::size_t>(static_cast<unsigned char>(at[1])) << 8) : u32(at));
        }
    } else {
        for (std::size_t i = 0; i + 2 < *position_count; i += 3) { indices.push_back(i); indices.push_back(i + 1); indices.push_back(i + 2); }
    }
    Mesh result;
    std::map<std::tuple<double, double, double>, std::size_t> unique_vertices;
    std::set<std::pair<std::size_t, std::size_t>> unique_edges;
    for (std::size_t i = 0; i < indices.size(); i += 3) {
        const auto a = vertex(indices[i]); const auto b = vertex(indices[i + 1]); const auto c = vertex(indices[i + 2]);
        if (!a || !b || !c || !append_triangle(result, unique_vertices, unique_edges, {*a, *b, *c})) return {};
    }
    result.source_path = path.generic_string();
    result.content_sha256 = core::sha256_file(path.string());
    return result;
}
} // namespace nadir::model
