#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace nadir::render {

class Framebuffer {
public:
    Framebuffer(int width, int height);
    int width() const;
    int height() const;
    void clear();
    void set(int x, int y, bool value = true);
    bool get(int x, int y) const;
    std::string braille(const std::string& ansi_color = "\x1b[38;2;0;255;119m") const;

private:
    int width_{};
    int height_{};
    std::vector<std::uint8_t> pixels_;
};

}
