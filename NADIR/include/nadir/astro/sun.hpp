#pragma once

#include <nadir/geo/eop.hpp>
#include <nadir/orbit/vector.hpp>

namespace nadir::astro {

// Low-precision analytical Earth-to-Sun vector (geocentric, mean equator of
// date). It is suitable for display/eclipse classification, not astrometry.
orbit::Vec3d sun_position_teme_km(double jd_utc) noexcept;
orbit::Vec3d sun_position_itrf_km(double jd_utc, const geo::EopRecord& eop) noexcept;

} // namespace nadir::astro
