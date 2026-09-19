#include <nadir/render/frame_clock.hpp>
#include <thread>
#include <algorithm>

namespace nadir::render {

FrameClock::FrameClock(double fps)
    : previous_(Clock::now()),
      next_(previous_),
      frame_duration_(
          std::chrono::duration_cast<Clock::duration>(
              std::chrono::duration<double>(1.0 / std::max(fps, 1.0))
          )
      ),
      target_fps_(std::max(fps, 1.0))
{
}

double FrameClock::tick()
{
    const auto now = Clock::now();

    const auto dt_duration = now - previous_;
    const double dt = std::chrono::duration<double>(dt_duration).count();

    previous_ = now;

    return std::clamp(dt, 0.0, 0.25);
}

void FrameClock::wait()
{
    const auto now = Clock::now();

    if (now > next_ + frame_duration_) {
        next_ = now + frame_duration_;
    } else {
        next_ += frame_duration_;
    }

    std::this_thread::sleep_until(next_);
}

double FrameClock::target_fps() const noexcept
{
    return target_fps_;
}

double FrameClock::frame_duration() const noexcept
{
    return std::chrono::duration<double>(frame_duration_).count();
}

void FrameClock::reset()
{
    previous_ = Clock::now();
    next_ = previous_;
}

}