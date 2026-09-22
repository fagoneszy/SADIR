#include <nadir/orbit/batch.hpp>
#include <nadir/orbit/sgp4.hpp>

#include <algorithm>
#include <thread>

namespace nadir::orbit {
PropagationBatch propagate_sgp4_batch(const std::vector<astro::OmmRecord>& records, double minutes,
                                      std::size_t worker_count) {
    PropagationBatch batch;
    const auto count = records.size();
    batch.norad_ids.resize(count);
    batch.position_x_km.resize(count); batch.position_y_km.resize(count); batch.position_z_km.resize(count);
    batch.velocity_x_km_s.resize(count); batch.velocity_y_km_s.resize(count); batch.velocity_z_km_s.resize(count);
    batch.valid.resize(count);
    if (count == 0) return batch;
    if (worker_count == 0) worker_count = std::thread::hardware_concurrency();
    worker_count = std::clamp<std::size_t>(worker_count == 0 ? 1 : worker_count, 1, count);
    const auto propagate_range = [&](std::size_t first, std::size_t last) {
        for (std::size_t index = first; index < last; ++index) {
            const auto state = propagate_sgp4(records[index], minutes);
            batch.norad_ids[index] = records[index].norad_cat_id;
            batch.valid[index] = static_cast<unsigned char>(static_cast<bool>(state));
            batch.position_x_km[index] = state.state.position_km.x;
            batch.position_y_km[index] = state.state.position_km.y;
            batch.position_z_km[index] = state.state.position_km.z;
            batch.velocity_x_km_s[index] = state.state.velocity_km_s.x;
            batch.velocity_y_km_s[index] = state.state.velocity_km_s.y;
            batch.velocity_z_km_s[index] = state.state.velocity_km_s.z;
        }
    };
    if (worker_count == 1) {
        propagate_range(0, count);
        return batch;
    }
    const auto chunk = (count + worker_count - 1) / worker_count;
    std::vector<std::thread> workers;
    workers.reserve(worker_count - 1);
    for (std::size_t worker{}; worker + 1 < worker_count; ++worker) {
        const auto first = worker * chunk;
        workers.emplace_back(propagate_range, first, std::min(count, first + chunk));
    }
    const auto final_first = (worker_count - 1) * chunk;
    propagate_range(final_first, count);
    for (auto& worker : workers) worker.join();
    return batch;
}
} // namespace nadir::orbit
