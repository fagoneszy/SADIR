#pragma once

#include <cstdint>

namespace nadir::state {

enum class StateKind : std::uint8_t {
    Unknown,
    Observed,
    Estimated,
    Propagated,
    Predicted,
    Reconstructed,
    Simulated
};

} // namespace nadir::state
