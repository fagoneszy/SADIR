#pragma once

#include <array>
#include <cstdint>

#include <nadir/time/instant.hpp>

namespace nadir::state {

struct Provenance {
    std::uint32_t source_id{};
    std::uint64_t source_record_id{};
    std::uint32_t model_id{};
    std::uint64_t derivation_id{};
    time::TimeInstant source_epoch{};
    time::TimeInstant ingested_at{};
    std::array<std::uint8_t, 32> content_sha256{};
    bool source_verified{};
};

} // namespace nadir::state
