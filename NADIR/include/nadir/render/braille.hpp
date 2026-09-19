#pragma once

#include <cstdint>

namespace nadir::render {

constexpr std::uint8_t braille_bit(int x, int y) noexcept {
    constexpr std::uint8_t bits[4][2] = {
        {0x01, 0x08},
        {0x02, 0x10},
        {0x04, 0x20},
        {0x40, 0x80}
    };
    return bits[y][x];
}

char32_t encode_braille(const float samples[8], float threshold = 0.5f) noexcept {
    std::uint8_t mask = 0;
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 2; ++x) {
            if (samples[y * 2 + x] >= threshold) {
                mask |= braille_bit(x, y);
            }
        }
    }
    return static_cast<char32_t>(0x2800 + mask);
}

char32_t encode_braille_cell(const float samples[2][4], float threshold = 0.5f) noexcept {
    std::uint8_t mask = 0;
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 2; ++x) {
            if (samples[x][y] >= threshold) {
                // Braille dot mapping:
                // dots 1-4 are left column (y=0..3, x=0)
                // dots 5-8 are right column (y=0..3, x=1)
                // Unicode Braille dots: 1=0x01, 2=0x02, 3=0x04, 4=0x08, 5=0x10, 6=0x20, 7=0x40, 8=0x80
                static const std::uint8_t bits[4][2] = {
                    {0x01, 0x08},  // dot 1, dot 5
                    {0x02, 0x10},  // dot 2, dot 6
                    {0x04, 0x20},  // dot 3, dot 7
                    {0x40, 0x80}   // dot 4, dot 8
                };
                mask |= bits[y][x];
            }
        }
    }
    return static_cast<char32_t>(0x2800 + mask);
}

}