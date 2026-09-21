#pragma once
#include <array>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <vector>
#include <nadir/orbit/numerical.hpp>

namespace nadir::format {

enum class NdrRecordType : std::uint16_t {
    Source = 1,
    Object = 2,
    State = 3,
    Event = 4,
};

struct NdrHeader {
    std::array<char, 8> magic{'N','A','D','I','R',0,0,2};
    std::uint16_t version{2};
    std::uint16_t type{};
    std::uint64_t timestamp_ns{};
    std::uint64_t record_count{};
    std::uint32_t flags{};
    std::uint32_t crc32{}; // CRC of every serialized record after this header.
};

// NDR uses an explicit little-endian wire representation; NdrHeader is never
// written with a raw struct write, so compiler padding cannot affect files.
struct NdrRecord {
    std::uint16_t type{};
    std::uint64_t timestamp_ns{};
    std::vector<std::uint8_t> payload;
    bool operator==(const NdrRecord&) const = default;
};

struct NdrStateBlock {
    std::uint64_t object_id{};
    orbit::CartesianState state{};
    bool operator==(const NdrStateBlock& rhs) const noexcept {
        return object_id == rhs.object_id && state.position_m.x == rhs.state.position_m.x &&
            state.position_m.y == rhs.state.position_m.y && state.position_m.z == rhs.state.position_m.z &&
            state.velocity_m_s.x == rhs.state.velocity_m_s.x && state.velocity_m_s.y == rhs.state.velocity_m_s.y &&
            state.velocity_m_s.z == rhs.state.velocity_m_s.z;
    }
};

struct NdrFile {
    NdrHeader header;
    std::vector<NdrRecord> records;
};

// Immutable seek index for replay clients. Files are written in chronological
// order; seek returns the latest record at or before the requested instant.
class NdrReplay {
public:
    explicit NdrReplay(NdrFile file);
    const NdrFile& file() const noexcept;
    const NdrRecord* seek(std::uint64_t timestamp_ns) const noexcept;
    std::span<const NdrRecord> records() const noexcept;
private:
    NdrFile file_;
};

std::uint32_t crc32(std::span<const std::uint8_t> bytes);
std::vector<std::uint8_t> encode_state_block(const NdrStateBlock& state);
std::optional<NdrStateBlock> decode_state_block(std::span<const std::uint8_t> bytes);
bool write_ndr(const std::filesystem::path& path, NdrHeader header,
               std::span<const NdrRecord> records);
std::optional<NdrFile> read_ndr(const std::filesystem::path& path);

// Compatibility helper for callers that need an empty NDR stream.
bool write_header(const std::filesystem::path& path, const NdrHeader& header);

}
