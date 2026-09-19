#pragma once

#include <cstdint>

namespace nadir::state {

enum class OriginKind : std::uint8_t {
    Unspecified,
    EarthCenter,
    SolarSystemBarycenter,
    CelestialBodyCenter,
    Observer,
    Spacecraft
};

struct ReferenceOrigin {
    OriginKind kind{OriginKind::Unspecified};
    std::uint64_t context_id{};

    constexpr bool valid() const noexcept {
        switch (kind) {
        case OriginKind::EarthCenter:
        case OriginKind::SolarSystemBarycenter:
            return true;
        case OriginKind::CelestialBodyCenter:
        case OriginKind::Observer:
        case OriginKind::Spacecraft:
            return context_id != 0;
        case OriginKind::Unspecified:
            return false;
        }
        return false;
    }

    constexpr bool operator==(const ReferenceOrigin&) const noexcept = default;
};

constexpr ReferenceOrigin earth_center() noexcept {
    return {OriginKind::EarthCenter, 0};
}

constexpr ReferenceOrigin solar_system_barycenter() noexcept {
    return {OriginKind::SolarSystemBarycenter, 0};
}

constexpr ReferenceOrigin body_center(std::uint64_t id) noexcept {
    return {OriginKind::CelestialBodyCenter, id};
}

} // namespace nadir::state
