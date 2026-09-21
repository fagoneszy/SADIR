#pragma once

#include <nadir/orbit/tracker.hpp>
#include <nadir/render/scene_builder.hpp>

namespace nadir::orbit {

// Adds an ITRF propagated object to a renderer snapshot without relabelling its
// frame or origin. The caller owns snapshot epoch/provenance policy.
bool add_tracking_scene_object(render::SceneBuilder& builder, std::uint64_t norad_id,
                               const TrackingResult& tracking, time::TimeInstant epoch,
                               state::StateQuality quality = state::StateQuality::Nominal);

} // namespace nadir::orbit
