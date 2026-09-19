#pragma once

#include <cstdint>
#include <string>

#include <nadir/state/tracked_state.hpp>

namespace nadir::render {

struct SceneObject {
    std::uint64_t entity_id{};
    std::string label;
    state::TrackedState state{};

    constexpr bool valid() const noexcept {
        return entity_id != 0 && state.valid();
    }
};

} // namespace nadir::render
