#include <nadir/format/ndr.hpp>

#include <array>
#include <fstream>
#include <limits>
#include <type_traits>

namespace nadir::format {
namespace {

constexpr std::size_t header_size = 8 + 2 + 2 + 8 + 8 + 4 + 4;
constexpr std::uint32_t max_payload_bytes = 64U * 1024U * 1024U;

template <class T>
void append_le(std::vector<std::uint8_t>& out, T value) {
    static_assert(std::is_integral_v<T>);
    using U = std::make_unsigned_t<T>;
    const auto bits = static_cast<U>(value);
    for (std::size_t i = 0; i < sizeof(T); ++i)
        out.push_back(static_cast<std::uint8_t>((bits >> (i * 8U)) & 0xffU));
}

template <class T>
bool take_le(std::span<const std::uint8_t> bytes, std::size_t& offset, T& value) {
    static_assert(std::is_integral_v<T>);
    if (bytes.size() - offset < sizeof(T)) return false;
    using U = std::make_unsigned_t<T>;
    U bits{};
    for (std::size_t i = 0; i < sizeof(T); ++i)
        bits |= static_cast<U>(bytes[offset++]) << (i * 8U);
    value = static_cast<T>(bits);
    return true;
}

void append_header(std::vector<std::uint8_t>& out, const NdrHeader& header) {
    out.insert(out.end(), header.magic.begin(), header.magic.end());
    append_le(out, header.version);
    append_le(out, header.type);
    append_le(out, header.timestamp_ns);
    append_le(out, header.record_count);
    append_le(out, header.flags);
    append_le(out, header.crc32);
}

bool take_header(std::span<const std::uint8_t> bytes, NdrHeader& header) {
    if (bytes.size() < header_size) return false;
    std::size_t offset{};
    for (auto& ch : header.magic) ch = static_cast<char>(bytes[offset++]);
    return take_le(bytes, offset, header.version) && take_le(bytes, offset, header.type) &&
           take_le(bytes, offset, header.timestamp_ns) && take_le(bytes, offset, header.record_count) &&
           take_le(bytes, offset, header.flags) && take_le(bytes, offset, header.crc32);
}

} // namespace

std::uint32_t crc32(std::span<const std::uint8_t> bytes) {
    std::uint32_t crc = 0xffffffffU;
    for (const auto byte : bytes) {
        crc ^= byte;
        for (int bit = 0; bit < 8; ++bit)
            crc = (crc >> 1U) ^ ((crc & 1U) ? 0xedb88320U : 0U);
    }
    return ~crc;
}

bool write_ndr(const std::filesystem::path& path, NdrHeader header,
               std::span<const NdrRecord> records) {
    std::vector<std::uint8_t> encoded;
    std::uint64_t previous_timestamp{};
    bool first = true;
    for (const auto& record : records) {
        if (!first && record.timestamp_ns < previous_timestamp) return false;
        if (record.payload.size() > max_payload_bytes) return false;
        append_le(encoded, record.type);
        append_le(encoded, record.timestamp_ns);
        append_le(encoded, static_cast<std::uint32_t>(record.payload.size()));
        encoded.insert(encoded.end(), record.payload.begin(), record.payload.end());
        previous_timestamp = record.timestamp_ns;
        first = false;
    }
    header.magic = {'N','A','D','I','R',0,0,2};
    header.version = 2;
    header.record_count = records.size();
    header.crc32 = crc32(encoded);

    std::vector<std::uint8_t> file;
    file.reserve(header_size + encoded.size());
    append_header(file, header);
    file.insert(file.end(), encoded.begin(), encoded.end());
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) return false;
    out.write(reinterpret_cast<const char*>(file.data()), static_cast<std::streamsize>(file.size()));
    return static_cast<bool>(out);
}

std::optional<NdrFile> read_ndr(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary | std::ios::ate);
    if (!input) return std::nullopt;
    const auto end = input.tellg();
    if (end < static_cast<std::streamoff>(header_size)) return std::nullopt;
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(end));
    input.seekg(0);
    input.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    if (!input) return std::nullopt;

    NdrFile result;
    const auto expected_magic = std::array<char, 8>{'N','A','D','I','R',0,0,2};
    if (!take_header(bytes, result.header) || result.header.magic != expected_magic || result.header.version != 2 ||
        result.header.crc32 != crc32(std::span<const std::uint8_t>{bytes}.subspan(header_size))) return std::nullopt;
    std::size_t offset = header_size;
    result.records.reserve(static_cast<std::size_t>(result.header.record_count));
    for (std::uint64_t i = 0; i < result.header.record_count; ++i) {
        NdrRecord record;
        std::uint32_t payload_size{};
        if (!take_le(bytes, offset, record.type) || !take_le(bytes, offset, record.timestamp_ns) ||
            !take_le(bytes, offset, payload_size) || payload_size > max_payload_bytes || bytes.size() - offset < payload_size) return std::nullopt;
        record.payload.assign(bytes.begin() + static_cast<std::ptrdiff_t>(offset),
                              bytes.begin() + static_cast<std::ptrdiff_t>(offset + payload_size));
        offset += payload_size;
        result.records.push_back(std::move(record));
    }
    if (offset != bytes.size()) return std::nullopt;
    return result;
}

NdrReplay::NdrReplay(NdrFile file) : file_(std::move(file)) {}

const NdrFile& NdrReplay::file() const noexcept { return file_; }

const NdrRecord* NdrReplay::seek(std::uint64_t timestamp_ns) const noexcept {
    const auto it = std::upper_bound(file_.records.begin(), file_.records.end(), timestamp_ns,
        [](std::uint64_t value, const NdrRecord& record) { return value < record.timestamp_ns; });
    return it == file_.records.begin() ? nullptr : &*std::prev(it);
}

std::span<const NdrRecord> NdrReplay::records() const noexcept { return file_.records; }

bool write_header(const std::filesystem::path& path, const NdrHeader& header) {
    return write_ndr(path, header, {});
}

}
