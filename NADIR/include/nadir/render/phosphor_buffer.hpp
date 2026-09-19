#pragma once

#include <vector>
#include <cstdint>

namespace nadir::render {

struct TerminalCell {
    char32_t glyph{U' '};
    std::uint8_t r{};
    std::uint8_t g{};
    std::uint8_t b{};
    std::uint8_t attributes{};
};

class PhosphorBuffer {
public:
    PhosphorBuffer(int width, int height);
    PhosphorBuffer(int width, int height, double half_life_s);

    void resize(int width, int height);
    void clear();

    void decay(double dt);
    void inject(int x, int y, float intensity);
    void inject_color(int x, int y, float intensity, std::uint8_t r, std::uint8_t g, std::uint8_t b);

    float get(int x, int y) const;
    TerminalCell get_cell(int x, int y) const;

    int width() const noexcept;
    int height() const noexcept;

private:
    int w_{};
    int h_{};
    double half_life_s_{0.25};
    std::vector<float> pixels_;
    std::vector<TerminalCell> cells_;
};

}