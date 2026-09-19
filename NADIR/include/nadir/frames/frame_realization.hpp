#pragma once

#include <cstdint>

namespace nadir::frames {

enum class TerrestrialRealization : std::uint16_t {
    Unspecified,
    ITRF2008,
    ITRF2014,
    ITRF2020
};

} // namespace nadir::frames
