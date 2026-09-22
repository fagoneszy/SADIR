#include <nadir/format/ndr.hpp>
#include <nadir/tui/app.hpp>

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

int main() {
    const auto path = std::filesystem::temp_directory_path() / "nadir-ndr-test.ndr";
    const nadir::format::NdrStateBlock state{42, {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}}};
    const auto decoded_state=nadir::format::decode_state_block(nadir::format::encode_state_block(state));
    if (!decoded_state || *decoded_state != state) return 1;
    const nadir::format::NdrSourceBlock source{"celestrak-gp", "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", 1234};
    const auto decoded_source = nadir::format::decode_source_block(nadir::format::encode_source_block(source));
    if (!decoded_source || *decoded_source != source) return 6;
    const nadir::format::NdrObjectBlock object{42, 25544, "ISS (ZARYA)"};
    const auto decoded_object = nadir::format::decode_object_block(nadir::format::encode_object_block(object));
    if (!decoded_object || *decoded_object != object) return 10;
    const nadir::format::NdrEventBlock event{42, "MANEUVER", "planned delta-v"};
    const auto decoded_event = nadir::format::decode_event_block(nadir::format::encode_event_block(event));
    if (!decoded_event || *decoded_event != event) return 12;
    auto malformed_source = source;
    malformed_source.sha256[0] = 'G';
    const std::array<std::uint8_t, 2> empty_source{0, 0};
    if (!nadir::format::encode_source_block(malformed_source).empty() ||
        nadir::format::decode_source_block(empty_source) ||
        !nadir::format::encode_object_block({0, 25544, "invalid"}).empty()) return 7;
    const std::vector<nadir::format::NdrRecord> records{
        {static_cast<std::uint16_t>(nadir::format::NdrRecordType::Source), 100, nadir::format::encode_source_block(source)},
        {static_cast<std::uint16_t>(nadir::format::NdrRecordType::Object), 150, nadir::format::encode_object_block(object)},
        {static_cast<std::uint16_t>(nadir::format::NdrRecordType::Event), 175, nadir::format::encode_event_block(event)},
        {static_cast<std::uint16_t>(nadir::format::NdrRecordType::State), 200, nadir::format::encode_state_block(state)},
    };
    if (!nadir::format::write_ndr(path, {.type = 7, .timestamp_ns = 42}, records)) return 1;
    const auto read = nadir::format::read_ndr(path);
    if (!read || read->header.type != 7 || read->header.timestamp_ns != 42 || read->records != records) return 2;
    std::ostringstream console;
    auto* const previous_console = std::cout.rdbuf(console.rdbuf());
    const int inspect_status = nadir::tui::App{}.run({"ndr", "inspect", path.string()});
    const int seek_status = nadir::tui::App{}.run({"ndr", "seek", path.string(), "150"});
    const int event_seek_status = nadir::tui::App{}.run({"ndr", "seek", path.string(), "175"});
    const int replay_status = nadir::tui::App{}.run({"ndr", "replay", path.string(), "150", "175"});
    std::cout.rdbuf(previous_console);
    if (inspect_status != 0 || seek_status != 0 || event_seek_status != 0 || replay_status != 0 || console.str().find("RECORDS 4") == std::string::npos ||
        console.str().find("NAME ISS (ZARYA)") == std::string::npos || console.str().find("EVENT MANEUVER") == std::string::npos ||
        console.str().find("REPLAYED 2") == std::string::npos) return 11;
    const nadir::format::NdrReplay replay(*read);
    if (replay.seek(99) || !replay.seek(100) || replay.seek(150)->type != static_cast<std::uint16_t>(nadir::format::NdrRecordType::Object) ||
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
    std::uint32_t random = 0x9e3779b9U;
    for (int sample{}; sample < 256; ++sample) {
        const auto length = random % 512U;
        random = random * 1664525U + 1013904223U;
        std::vector<std::uint8_t> fuzz(length);
        for (auto& byte : fuzz) { random = random * 1664525U + 1013904223U; byte = static_cast<std::uint8_t>(random >> 24U); }
        { std::ofstream output(path, std::ios::binary | std::ios::trunc); output.write(reinterpret_cast<const char*>(fuzz.data()), static_cast<std::streamsize>(fuzz.size())); }
        (void)nadir::format::read_ndr(path);
        (void)nadir::format::decode_source_block(fuzz);
        (void)nadir::format::decode_object_block(fuzz);
        (void)nadir::format::decode_state_block(fuzz);
        (void)nadir::format::decode_event_block(fuzz);
    }
    std::filesystem::remove(path);
    return 0;
}
