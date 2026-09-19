#pragma once

#include <cstdint>

namespace nadir::orbit {

enum class Frame : std::uint8_t {
    Unknown,
    TEME,
    PEF,
    ITRF,
    GCRF,
    ENU
};

}