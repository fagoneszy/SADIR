#pragma once
#include <array>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <vector>

namespace nadir::format {

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

struct NdrFile {
    NdrHeader header;
    std::vector<NdrRecord> records;
};

std::uint32_t crc32(std::span<const std::uint8_t> bytes);
bool write_ndr(const std::filesystem::path& path, NdrHeader header,
               std::span<const NdrRecord> records);
std::optional<NdrFile> read_ndr(const std::filesystem::path& path);

// Compatibility helper for callers that need an empty NDR stream.
bool write_header(const std::filesystem::path& path, const NdrHeader& header);

}
