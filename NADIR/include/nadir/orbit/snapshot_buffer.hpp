#pragma once

#include <cstdint>
#include <mutex>

#include <nadir/orbit/batch.hpp>

namespace nadir::orbit {

struct OrbitSnapshot {
    std::uint64_t sequence{};
    PropagationBatch batch;
};

// Producer builds an inactive buffer and publishes it in one locked swap.
// Consumers receive an owning copy, never a partially-mutated vector set.
class OrbitSnapshotBuffer {
public:
    void publish(PropagationBatch batch);
    [[nodiscard]] OrbitSnapshot read() const;
private:
    mutable std::mutex mutex_;
    OrbitSnapshot front_{};
    OrbitSnapshot back_{};
};

} // namespace nadir::orbit
