#include <nadir/format/ndr.hpp>

#include <array>
#include <filesystem>
#include <fstream>
#include <vector>

int main() {
    const auto path = std::filesystem::temp_directory_path() / "nadir-ndr-test.ndr";
    const nadir::format::NdrStateBlock state{42, {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}}};
    const auto decoded_state=nadir::format::decode_state_block(nadir::format::encode_state_block(state));
    if (!decoded_state || *decoded_state != state) return 1;
    const nadir::format::NdrSourceBlock source{"celestrak-gp", "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", 1234};
    const auto decoded_source = nadir::format::decode_source_block(nadir::format::encode_source_block(source));
    if (!decoded_source || *decoded_source != source) return 6;
    auto malformed_source = source;
    malformed_source.sha256[0] = 'G';
    const std::array<std::uint8_t, 2> empty_source{0, 0};
    if (!nadir::format::encode_source_block(malformed_source).empty() ||
        nadir::format::decode_source_block(empty_source)) return 7;
    const std::vector<nadir::format::NdrRecord> records{
        {static_cast<std::uint16_t>(nadir::format::NdrRecordType::Source), 100, nadir::format::encode_source_block(source)},
        {static_cast<std::uint16_t>(nadir::format::NdrRecordType::State), 200, nadir::format::encode_state_block(state)},
    };
    if (!nadir::format::write_ndr(path, {.type = 7, .timestamp_ns = 42}, records)) return 1;
    const auto read = nadir::format::read_ndr(path);
    if (!read || read->header.type != 7 || read->header.timestamp_ns != 42 || read->records != records) return 2;
    const nadir::format::NdrReplay replay(*read);
    if (replay.seek(99) || !replay.seek(100) || replay.seek(150)->type != static_cast<std::uint16_t>(nadir::format::NdrRecordType::Source) ||
        replay.seek(999)->type != static_cast<std::uint16_t>(nadir::format::NdrRecordType::State)) return 3;
    std::fstream oversized_count(path, std::ios::in | std::ios::out | std::ios::binary);
    oversized_count.seekp(20, std::ios::beg); // record_count in the fixed NDR v2 header
    oversized_count.put(static_cast<char>(0xff));
    oversized_count.close();
    if (nadir::format::read_ndr(path)) return 8;
    if (!nadir::format::write_ndr(path, {.type = 7, .timestamp_ns = 42}, records)) return 9;
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
