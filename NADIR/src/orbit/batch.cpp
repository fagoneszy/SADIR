#include <nadir/orbit/batch.hpp>
#include <nadir/orbit/sgp4.hpp>

namespace nadir::orbit {
PropagationBatch propagate_sgp4_batch(const std::vector<astro::OmmRecord>& records, double minutes) {
    PropagationBatch batch;
    const auto reserve = [&] (auto& values) { values.reserve(records.size()); };
    reserve(batch.norad_ids); reserve(batch.position_x_km); reserve(batch.position_y_km); reserve(batch.position_z_km);
    reserve(batch.velocity_x_km_s); reserve(batch.velocity_y_km_s); reserve(batch.velocity_z_km_s); reserve(batch.valid);
    for (const auto& record : records) {
        const auto state = propagate_sgp4(record, minutes);
        batch.norad_ids.push_back(record.norad_cat_id);
        batch.valid.push_back(static_cast<unsigned char>(static_cast<bool>(state)));
        batch.position_x_km.push_back(state.state.position_km.x); batch.position_y_km.push_back(state.state.position_km.y); batch.position_z_km.push_back(state.state.position_km.z);
        batch.velocity_x_km_s.push_back(state.state.velocity_km_s.x); batch.velocity_y_km_s.push_back(state.state.velocity_km_s.y); batch.velocity_z_km_s.push_back(state.state.velocity_km_s.z);
    }
    return batch;
}
} // namespace nadir::orbit
