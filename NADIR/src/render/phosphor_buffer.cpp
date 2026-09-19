#include <nadir/render/phosphor_buffer.hpp>
#include <cmath>
#include <algorithm>

namespace nadir::render {

PhosphorBuffer::PhosphorBuffer(int width, int height)
    : w_(width), h_(height),
      half_life_s_(0.25),
      pixels_(width * height, 0.0f),
      cells_(width * height)
{
}

PhosphorBuffer::PhosphorBuffer(int width, int height, double half_life_s)
    : w_(width), h_(height),
      half_life_s_(half_life_s),
      pixels_(width * height, 0.0f),
      cells_(width * height)
{
}

void PhosphorBuffer::resize(int width, int height) {
    w_ = width;
    h_ = height;
    pixels_.assign(width * height, 0.0f);
    cells_.assign(width * height, TerminalCell{});
}

void PhosphorBuffer::clear() {
    std::fill(pixels_.begin(), pixels_.end(), 0.0f);
    std::fill(cells_.begin(), cells_.end(), TerminalCell{});
}

float phosphor_decay(double dt, double half_life) {
    if (half_life <= 0.0) return 0.0f;
    return static_cast<float>(std::pow(0.5, dt / half_life));
}

void PhosphorBuffer::decay(double dt) {
    const float factor = phosphor_decay(dt, half_life_s_);
    for (auto& v : pixels_) {
        v *= factor;
        if (v < 0.001f) v = 0.0f;
    }
}

void PhosphorBuffer::inject(int x, int y, float intensity) {
    if (x < 0 || x >= w_ || y < 0 || y >= h_) return;
    const int idx = y * w_ + x;
    pixels_[idx] = std::max(pixels_[idx], intensity);
}

void PhosphorBuffer::inject_color(int x, int y, float intensity,
                                   std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    if (x < 0 || x >= w_ || y < 0 || y >= h_) return;
    const int idx = y * w_ + x;
    pixels_[idx] = std::max(pixels_[idx], intensity);
    auto& cell = cells_[idx];
    cell.r = r;
    cell.g = g;
    cell.b = b;
}

float PhosphorBuffer::get(int x, int y) const {
    if (x < 0 || x >= w_ || y < 0 || y >= h_) return 0.0f;
    return pixels_[y * w_ + x];
}

TerminalCell PhosphorBuffer::get_cell(int x, int y) const {
    if (x < 0 || x >= w_ || y < 0 || y >= h_) return TerminalCell{};
    return cells_[y * w_ + x];
}

int PhosphorBuffer::width() const noexcept { return w_; }
int PhosphorBuffer::height() const noexcept { return h_; }

}