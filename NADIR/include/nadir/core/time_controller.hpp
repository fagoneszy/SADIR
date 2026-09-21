#pragma once

#include <cstdint>
#include <chrono>

#include <nadir/core/time.hpp>

namespace nadir::core {

class TimeController {
public:
    TimeController();

    TimeState now() const;

    void freeze();
    void unfreeze();

    void set_utc_ns(std::int64_t value);
    void set_offset_ns(std::int64_t value);
    void set_rate(double value);

    bool frozen() const noexcept;
    std::int64_t offset_ns() const noexcept;
    double rate() const noexcept;

private:
    std::int64_t running_utc_ns() const;
    void anchor(std::int64_t utc_ns);

    bool frozen_{false};
    std::int64_t frozen_utc_ns_{};
    std::int64_t offset_ns_{};
    double rate_{1.0};
    std::int64_t anchor_utc_ns_{};
    std::chrono::steady_clock::time_point anchor_steady_{};
};

} // namespace nadir::core
