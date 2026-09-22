#pragma once

#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <thread>
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
                                      double minutes_since_epoch,
                                      std::size_t worker_count = 0);

// Reusable fixed-size worker pool for callers that propagate catalog snapshots
// repeatedly. `propagate` is synchronous and returns rows in input order.
class Sgp4BatchWorkerPool {
public:
    explicit Sgp4BatchWorkerPool(std::size_t worker_count = 0);
    ~Sgp4BatchWorkerPool();
    Sgp4BatchWorkerPool(const Sgp4BatchWorkerPool&) = delete;
    Sgp4BatchWorkerPool& operator=(const Sgp4BatchWorkerPool&) = delete;

    [[nodiscard]] std::size_t worker_count() const noexcept { return workers_.size(); }
    PropagationBatch propagate(const std::vector<astro::OmmRecord>& records, double minutes_since_epoch);

private:
    void worker(std::size_t worker_index);
    std::vector<std::thread> workers_;
    std::mutex invocation_mutex_;
    std::mutex mutex_;
    std::condition_variable work_ready_;
    std::condition_variable work_done_;
    const std::vector<astro::OmmRecord>* records_{};
    PropagationBatch* batch_{};
    double minutes_{};
    std::size_t generation_{};
    std::size_t completed_{};
    bool stopping_{};
};

} // namespace nadir::orbit
