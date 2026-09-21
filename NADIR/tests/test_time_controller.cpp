#include <nadir/core/time_controller.hpp>

#include <chrono>
#include <thread>

int main() {
    nadir::core::TimeController clock;
    clock.set_utc_ns(1'000'000'000LL);
    const auto set = clock.now().unix_ns;
    if (set < 999'000'000LL || set > 1'001'000'000LL) return 1;
    clock.freeze();
    const auto frozen = clock.now().unix_ns;
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    if (!clock.frozen() || clock.now().unix_ns != frozen) return 2;
    clock.set_rate(2.0);
    clock.unfreeze();
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    if (clock.now().unix_ns <= frozen || clock.rate() != 2.0) return 3;
    clock.set_offset_ns(123);
    if (clock.offset_ns() != 123) return 4;
    return 0;
}
