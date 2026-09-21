#include <nadir/format/ndr.hpp>

#include <filesystem>
#include <fstream>
#include <vector>

int main() {
    const auto path = std::filesystem::temp_directory_path() / "nadir-ndr-test.ndr";
    const std::vector<nadir::format::NdrRecord> records{{10, 100, {1, 2, 3}}, {11, 200, {9, 8}}};
    const nadir::format::NdrStateBlock state{42, {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}}};
    const auto decoded_state=nadir::format::decode_state_block(nadir::format::encode_state_block(state));
    if (!decoded_state || *decoded_state != state) return 1;
    if (!nadir::format::write_ndr(path, {.type = 7, .timestamp_ns = 42}, records)) return 1;
    const auto read = nadir::format::read_ndr(path);
    if (!read || read->header.type != 7 || read->header.timestamp_ns != 42 || read->records != records) return 2;
    const nadir::format::NdrReplay replay(*read);
    if (replay.seek(99) || !replay.seek(100) || replay.seek(150)->type != 10 || replay.seek(999)->type != 11) return 3;
    const std::vector<nadir::format::NdrRecord> out_of_order{{1, 2, {}}, {1, 1, {}}};
    if (nadir::format::write_ndr(path, {}, out_of_order)) return 4;
    std::fstream corrupt(path, std::ios::in | std::ios::out | std::ios::binary);
    corrupt.seekp(-1, std::ios::end);
    corrupt.put('\0');
    corrupt.close();
    if (nadir::format::read_ndr(path)) return 5;
    std::filesystem::remove(path);
    return 0;
}
