#include <nadir/orbit/snapshot_buffer.hpp>

int main() {
    nadir::orbit::OrbitSnapshotBuffer buffer;
    if (buffer.read().sequence != 0) return 1;
    nadir::orbit::PropagationBatch first{}; first.norad_ids = {1}; first.valid = {1};
    buffer.publish(first);
    const auto a = buffer.read();
    if (a.sequence != 1 || a.batch.norad_ids != std::vector<std::uint64_t>{1}) return 2;
    first.norad_ids[0] = 99;
    if (buffer.read().batch.norad_ids[0] != 1) return 3;
    nadir::orbit::PropagationBatch second{}; second.norad_ids = {2, 3}; second.valid = {1, 1};
    buffer.publish(std::move(second));
    const auto b = buffer.read();
    return b.sequence == 2 && b.batch.norad_ids.size() == 2 && b.batch.norad_ids[0] == 2 ? 0 : 4;
}
