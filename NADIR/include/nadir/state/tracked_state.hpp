#pragma once

#include <cstdint>

#include <nadir/state/covariance.hpp>
#include <nadir/state/freshness.hpp>
#include <nadir/state/physical_state.hpp>
#include <nadir/state/provenance.hpp>
#include <nadir/state/state_kind.hpp>
#include <nadir/state/state_quality.hpp>

namespace nadir::state {

struct TrackedState {
    std::uint64_t object_id{};
    PhysicalState physical{};
    StateKind kind{StateKind::Unknown};
    StateQuality quality{StateQuality::Unknown};
    Provenance provenance{};
    Freshness freshness{};
    StateUncertainty uncertainty{};

    constexpr bool valid() const noexcept {
        return object_id != 0 && physical.valid() && quality != StateQuality::Invalid;
    }
};

} // namespace nadir::state
