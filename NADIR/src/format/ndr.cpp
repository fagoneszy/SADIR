#include <nadir/format/ndr.hpp>

namespace nadir::format {

bool write_header(const std::filesystem::path& path, const NdrHeader& header) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    return static_cast<bool>(out);
}

}
