#pragma once

#include <nadir/frames/reference_frame.hpp>
#include <nadir/state/reference_origin.hpp>
#include <nadir/time/instant.hpp>
#include <nadir/units/vector_units.hpp>

namespace nadir::state {

constexpr bool frame_origin_compatible(
    const frames::ReferenceFrame& frame,
    const ReferenceOrigin& origin) noexcept {
    if (!frame.valid() || !origin.valid()) return false;

    switch (frame.kind) {
    case frames::FrameKind::ENU:
    case frames::FrameKind::NED:
        return origin.kind == OriginKind::Observer && origin.context_id == frame.context.id;
    case frames::FrameKind::BodyFixed:
        return origin.kind == OriginKind::CelestialBodyCenter && origin.context_id == frame.context.id;
    case frames::FrameKind::SpacecraftBody:
        return origin.kind == OriginKind::Spacecraft && origin.context_id == frame.context.id;
    case frames::FrameKind::TEME:
    case frames::FrameKind::PEF:
    case frames::FrameKind::ITRF:
    case frames::FrameKind::GCRF:
    case frames::FrameKind::CIRS:
    case frames::FrameKind::TIRS:
        return origin.kind == OriginKind::EarthCenter;
    case frames::FrameKind::ICRF:
        return origin.kind == OriginKind::SolarSystemBarycenter;
    case frames::FrameKind::Unknown:
        return false;
    }
    return false;
}

struct PhysicalState {
    units::PositionMeters position{};
    units::VelocityMetersPerSecond velocity{};
    time::TimeInstant epoch{};
    frames::ReferenceFrame frame{};
    ReferenceOrigin origin{};

    constexpr bool valid() const noexcept {
        return frame_origin_compatible(frame, origin);
    }
};

} // namespace nadir::state
