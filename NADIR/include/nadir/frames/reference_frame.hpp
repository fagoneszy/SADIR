#pragma once

#include <cstdint>

#include <nadir/frames/frame_context.hpp>
#include <nadir/frames/frame_realization.hpp>

namespace nadir::frames {

enum class FrameKind : std::uint16_t {
    Unknown,
    ICRF,
    GCRF,
    CIRS,
    TIRS,
    TEME,
    PEF,
    ITRF,
    ENU,
    NED,
    BodyFixed,
    SpacecraftBody
};

struct ReferenceFrame {
    FrameKind kind{FrameKind::Unknown};
    FrameContext context{};
    TerrestrialRealization terrestrial{TerrestrialRealization::Unspecified};

    constexpr bool valid() const noexcept {
        if (kind == FrameKind::Unknown) return false;

        if (kind == FrameKind::ENU || kind == FrameKind::NED) {
            return context.kind == FrameContextKind::Observer && context.id != 0;
        }
        if (kind == FrameKind::BodyFixed) {
            return context.kind == FrameContextKind::CelestialBody && context.id != 0;
        }
        if (kind == FrameKind::SpacecraftBody) {
            return context.kind == FrameContextKind::Spacecraft && context.id != 0;
        }
        return true;
    }

    constexpr bool operator==(const ReferenceFrame&) const noexcept = default;
};

constexpr ReferenceFrame teme() noexcept {
    return {FrameKind::TEME};
}

constexpr ReferenceFrame pef() noexcept {
    return {FrameKind::PEF};
}

constexpr ReferenceFrame gcrf() noexcept {
    return {FrameKind::GCRF};
}

constexpr ReferenceFrame icrf() noexcept {
    return {FrameKind::ICRF};
}

constexpr ReferenceFrame itrf2020() noexcept {
    return {FrameKind::ITRF, {}, TerrestrialRealization::ITRF2020};
}

constexpr ReferenceFrame enu(std::uint64_t observer_id) noexcept {
    return {FrameKind::ENU, {FrameContextKind::Observer, observer_id}};
}

constexpr ReferenceFrame body_fixed(std::uint64_t body_id) noexcept {
    return {FrameKind::BodyFixed, {FrameContextKind::CelestialBody, body_id}};
}

} // namespace nadir::frames
