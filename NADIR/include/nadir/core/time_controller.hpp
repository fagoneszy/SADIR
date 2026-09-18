#pragma once

#include <cstdint>

#include <nadir/core/time.hpp>

namespace nadir::core {

class TimeController {
public:
    TimeState now() const;

    void freeze();
    void unfreeze();

    void set_utc_ns(std::int64_t value);
    void set_offset_ns(std::int64_t value);

    bool frozen() const noexcept;
    std::int64_t offset_ns() const noexcept;

private:
    bool frozen_{false};
    std::int64_t frozen_utc_ns_{};
    std::int64_t offset_ns_{};
};

} // namespace nadir::core