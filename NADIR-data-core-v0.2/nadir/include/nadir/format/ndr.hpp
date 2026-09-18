#pragma once
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>

namespace nadir::format {

struct NdrHeader {
    std::array<char, 8> magic{'N','A','D','I','R',0,0,1};
    std::uint16_t version{1};
    std::uint16_t type{};
    std::uint64_t timestamp_ns{};
    std::uint64_t record_count{};
    std::uint32_t flags{};
    std::uint32_t crc32{};
};

bool write_header(const std::filesystem::path& path, const NdrHeader& header);

}
