#pragma once
#include <array>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <vector>
#include <nadir/orbit/numerical.hpp>

namespace nadir::format {

inline constexpr std::uint64_t max_ndr_file_bytes = 256ULL * 1024ULL * 1024ULL;
inline constexpr std::uint64_t max_ndr_records = 1'000'000ULL;

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

// Immutable reference to the raw cached payload that produced later records.
// sha256 is lowercase hexadecimal (64 characters); bytes is the raw payload size.
struct NdrSourceBlock {
    std::string source_id;
    std::string sha256;
    std::uint64_t bytes{};
    bool operator==(const NdrSourceBlock&) const = default;
};

// Stable object identity used by State and Event records in the same stream.
struct NdrObjectBlock {
    std::uint64_t object_id{};
    std::uint64_t catalog_id{};
    std::string name;
    bool operator==(const NdrObjectBlock&) const = default;
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
std::vector<std::uint8_t> encode_source_block(const NdrSourceBlock& source);
std::optional<NdrSourceBlock> decode_source_block(std::span<const std::uint8_t> bytes);
std::vector<std::uint8_t> encode_object_block(const NdrObjectBlock& object);
std::optional<NdrObjectBlock> decode_object_block(std::span<const std::uint8_t> bytes);
std::vector<std::uint8_t> encode_state_block(const NdrStateBlock& state);
std::optional<NdrStateBlock> decode_state_block(std::span<const std::uint8_t> bytes);
bool write_ndr(const std::filesystem::path& path, NdrHeader header,
               std::span<const NdrRecord> records);
std::optional<NdrFile> read_ndr(const std::filesystem::path& path);

// Compatibility helper for callers that need an empty NDR stream.
bool write_header(const std::filesystem::path& path, const NdrHeader& header);

}
