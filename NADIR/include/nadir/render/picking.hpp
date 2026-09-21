#pragma once

#include <cstdint>
#include <optional>

#include <nadir/render/camera.hpp>
#include <nadir/render/display_transform.hpp>
#include <nadir/render/scene.hpp>

namespace nadir::render {

struct PickResult {
    std::uint64_t entity_id{};
    double distance_pixels{};
    double depth{};
};

// Selects the nearest visible point inside radius_pixels. Equal distances are
// resolved by entity ID, making repeated terminal picks stable.
std::optional<PickResult> pick_scene(const SceneSnapshot& scene, const Camera& camera,
                                     const DisplayTransform& transform, int viewport_width,
                                     int viewport_height, double x, double y,
                                     double radius_pixels = 12.0);

} // namespace nadir::render
