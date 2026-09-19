#pragma once

#include <chrono>
#include <algorithm>

namespace nadir::render {

class FrameClock {
public:
    explicit FrameClock(double fps = 30.0);

    double tick();
    void wait();

    double target_fps() const noexcept;
    double frame_duration() const noexcept;

    void reset();

private:
    using Clock = std::chrono::steady_clock;

    Clock::time_point previous_;
    Clock::time_point next_;
    Clock::duration frame_duration_;
    double target_fps_;
};

}