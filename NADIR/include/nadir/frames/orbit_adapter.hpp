#pragma once

#include <nadir/frames/reference_frame.hpp>
#include <nadir/orbit/frame.hpp>

namespace nadir::orbit {

constexpr frames::ReferenceFrame to_reference_frame(Frame frame) noexcept {
    switch (frame) {
    case Frame::TEME: return frames::teme();
    case Frame::PEF: return frames::pef();
    case Frame::ITRF: return frames::itrf2020();
    case Frame::GCRF: return frames::gcrf();
    // Legacy ENU has no observer identity, so it cannot be represented honestly.
    case Frame::ENU:
    case Frame::Unknown: return {};
    }
    return {};
}

} // namespace nadir::orbit
