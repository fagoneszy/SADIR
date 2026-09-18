#include <nadir/render/framebuffer.hpp>
#include <array>

namespace nadir::render {

static void append_utf8(std::string& out, std::uint32_t cp) {
    if (cp <= 0x7F) {
        out.push_back(static_cast<char>(cp));
    } else if (cp <= 0x7FF) {
        out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else {
        out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }
}

Framebuffer::Framebuffer(int width, int height) : width_(width), height_(height), pixels_(static_cast<std::size_t>(width * height)) {}
int Framebuffer::width() const { return width_; }
int Framebuffer::height() const { return height_; }
void Framebuffer::clear() { std::fill(pixels_.begin(), pixels_.end(), 0); }

void Framebuffer::set(int x, int y, bool value) {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) return;
    pixels_[static_cast<std::size_t>(y * width_ + x)] = value ? 1 : 0;
}

bool Framebuffer::get(int x, int y) const {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) return false;
    return pixels_[static_cast<std::size_t>(y * width_ + x)] != 0;
}

std::string Framebuffer::braille(const std::string& ansi_color) const {
    static constexpr std::array<std::array<std::uint8_t, 4>, 2> bits{{{{0, 1, 2, 6}}, {{3, 4, 5, 7}}}};
    std::string out;
    out += ansi_color;
    for (int y = 0; y < height_; y += 4) {
        for (int x = 0; x < width_; x += 2) {
            std::uint8_t mask = 0;
            for (int dx = 0; dx < 2; ++dx) {
                for (int dy = 0; dy < 4; ++dy) {
                    if (get(x + dx, y + dy)) mask |= static_cast<std::uint8_t>(1u << bits[dx][dy]);
                }
            }
            append_utf8(out, 0x2800u + mask);
        }
        out += "\x1b[0m\n";
        out += ansi_color;
    }
    out += "\x1b[0m";
    return out;
}

}
