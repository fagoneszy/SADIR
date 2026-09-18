#include <cassert>
#include <iostream>

#include "nadir/time/time_state.hpp"
#include "nadir/time/time_scale_enum.hpp"
#include "nadir/time/clock.hpp"

int main() {
    // Test 1: TimeScale enum and string conversion
    using namespace nadir::time;
    assert(scale_to_string(TimeScale::UTC) == "UTC");
    assert(scale_to_string(TimeScale::TAI) == "TAI");
    assert(scale_to_string(TimeScale::GPS) == "GPS");
    std::cout << "PASS: TimeScale enum and conversion" << std::endl;

    // Test 2: TimeState defaults
    TimeState s;
    assert(s.utc_ns == 0);
    assert(s.monotonic_ns == 0);
    assert(s.dut1 == 0.0);
    assert(s.jd_utc == 0.0);
    std::cout << "PASS: TimeState defaults" << std::endl;

    // Test 3: Clock creation and validity
    Clock clock;
    assert(clock.is_valid());  // Should be valid after default construction
    std::cout << "PASS: Clock creation and validity" << std::endl;

    // Test 4: Clock tick
    clock.tick();
    auto state = clock.get_state();
    (void)state; // Avoid unused warning
    std::cout << "PASS: Clock tick" << std::endl;

    // Test 5: Clock freeze/unfreeze
    clock.freeze();
    assert(clock.is_frozen());
    clock.unfreeze();
    assert(!clock.is_frozen());
    std::cout << "PASS: Clock freeze/unfreeze" << std::endl;

    // Test 6: Clock UTC from epoch
    // 1 hour = 3600 seconds = 3,600,000,000,000 ns
    clock.set_utc_from_epoch(3'600'000'000'000LL);
    state = clock.get_state();
    assert(state.utc_ns == 3'600'000'000'000LL);
    std::cout << "PASS: Clock UTC from epoch" << std::endl;

    // Test 7: Clock to_c_time_t
    auto ct = clock.to_c_time_t();
    (void)ct;
    std::cout << "PASS: Clock to_c_time_t" << std::endl;

    // Test 8: Modified Julian Date string
    TimeState s2;
    s2.modified_julian_date();  // Just test it doesn't crash
    std::cout << "PASS: Modified Julian Date" << std::endl;

    std::cout << "\nAll Clock tests passed!" << std::endl;
    return 0;
}