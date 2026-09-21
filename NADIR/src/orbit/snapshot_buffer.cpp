#include <nadir/orbit/snapshot_buffer.hpp>

#include <utility>

namespace nadir::orbit {
void OrbitSnapshotBuffer::publish(PropagationBatch batch) {
    std::lock_guard lock(mutex_);
    back_.sequence = front_.sequence + 1;
    back_.batch = std::move(batch);
    std::swap(front_, back_);
}

OrbitSnapshot OrbitSnapshotBuffer::read() const {
    std::lock_guard lock(mutex_);
    return front_;
}
} // namespace nadir::orbit
