#pragma once

#include <cstdint>
#include <vector>

#include <nadir/astro/omm.hpp>

namespace nadir::orbit {

// Structure-of-arrays result, suitable for compact catalog snapshots and SIMD
// consumers. Row i is identified by norad_ids[i].
struct PropagationBatch {
    std::vector<std::uint64_t> norad_ids;
    std::vector<double> position_x_km, position_y_km, position_z_km;
    std::vector<double> velocity_x_km_s, velocity_y_km_s, velocity_z_km_s;
    std::vector<unsigned char> valid;
    [[nodiscard]] std::size_t size() const noexcept { return norad_ids.size(); }
};

PropagationBatch propagate_sgp4_batch(const std::vector<astro::OmmRecord>& records,
                                      double minutes_since_epoch);

} // namespace nadir::orbit
