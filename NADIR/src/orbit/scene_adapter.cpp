#include <nadir/orbit/scene_adapter.hpp>

namespace nadir::orbit {

bool add_tracking_scene_object(render::SceneBuilder& builder, std::uint64_t norad_id,
                               const TrackingResult& tracking, time::TimeInstant epoch,
                               state::StateQuality quality) {
    if (!tracking || norad_id == 0) return false;
    const state::PhysicalState physical{
        {{tracking.itrf_m.position.x, tracking.itrf_m.position.y, tracking.itrf_m.position.z}},
        {{tracking.itrf_m.velocity.x, tracking.itrf_m.velocity.y, tracking.itrf_m.velocity.z}},
        epoch, frames::itrf2020(), state::earth_center()};
    return builder.add_object({norad_id, physical, state::StateKind::Propagated, quality});
}

} // namespace nadir::orbit
