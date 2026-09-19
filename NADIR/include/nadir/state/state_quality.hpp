#pragma once

#include <cstdint>

namespace nadir::state {

enum class StateQuality : std::uint8_t {
    Unknown,
    Nominal,
    Degraded,
    Stale,
    Invalid
};

} // namespace nadir::state
