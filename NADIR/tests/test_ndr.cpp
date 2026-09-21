#include <nadir/format/ndr.hpp>

#include <filesystem>
#include <fstream>
#include <vector>

int main() {
    const auto path = std::filesystem::temp_directory_path() / "nadir-ndr-test.ndr";
    const std::vector<nadir::format::NdrRecord> records{{10, 100, {1, 2, 3}}, {11, 200, {9, 8}}};
    if (!nadir::format::write_ndr(path, {.type = 7, .timestamp_ns = 42}, records)) return 1;
    const auto read = nadir::format::read_ndr(path);
    if (!read || read->header.type != 7 || read->header.timestamp_ns != 42 || read->records != records) return 2;
    std::fstream corrupt(path, std::ios::in | std::ios::out | std::ios::binary);
    corrupt.seekp(-1, std::ios::end);
    corrupt.put('\0');
    corrupt.close();
    if (nadir::format::read_ndr(path)) return 3;
    std::filesystem::remove(path);
    return 0;
}
