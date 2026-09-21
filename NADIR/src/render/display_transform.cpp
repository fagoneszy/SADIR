#include <nadir/render/display_transform.hpp>
namespace nadir::render { math::Vec3d to_render_space(const math::Vec3d& p,const DisplayTransform& t) noexcept { return t.meters_per_render_unit>0.0 ? (p-t.physical_origin_m)/t.meters_per_render_unit : math::Vec3d{}; } }
