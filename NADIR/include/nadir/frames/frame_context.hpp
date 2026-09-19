#pragma once

#include <cstdint>

namespace nadir::frames {

enum class FrameContextKind : std::uint8_t {
    None,
    Observer,
    CelestialBody,
    Spacecraft,
    TerrestrialRealization
};

struct FrameContext {
    FrameContextKind kind{FrameContextKind::None};
    std::uint64_t id{};

    constexpr bool operator==(const FrameContext&) const noexcept = default;
};

} // namespace nadir::frames
