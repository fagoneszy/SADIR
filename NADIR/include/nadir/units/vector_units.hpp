#pragma once

#include <nadir/math/vec3.hpp>

namespace nadir::units {

struct PositionMeters {
    math::Vec3d value{};
};

struct PositionKilometers {
    math::Vec3d value{};
};

struct VelocityMetersPerSecond {
    math::Vec3d value{};
};

struct VelocityKilometersPerSecond {
    math::Vec3d value{};
};

constexpr PositionMeters to_meters(PositionKilometers position) noexcept {
    return PositionMeters{{
        position.value.x * 1000.0,
        position.value.y * 1000.0,
        position.value.z * 1000.0
    }};
}

constexpr PositionKilometers to_kilometers(PositionMeters position) noexcept {
    return PositionKilometers{{
        position.value.x / 1000.0,
        position.value.y / 1000.0,
        position.value.z / 1000.0
    }};
}

constexpr VelocityMetersPerSecond to_meters_per_second(
    VelocityKilometersPerSecond velocity) noexcept {
    return VelocityMetersPerSecond{{
        velocity.value.x * 1000.0,
        velocity.value.y * 1000.0,
        velocity.value.z * 1000.0
    }};
}

constexpr VelocityKilometersPerSecond to_kilometers_per_second(
    VelocityMetersPerSecond velocity) noexcept {
    return VelocityKilometersPerSecond{{
        velocity.value.x / 1000.0,
        velocity.value.y / 1000.0,
        velocity.value.z / 1000.0
    }};
}

} // namespace nadir::units
