#include <nadir/render/presenter.hpp>
#include <nadir/render/phosphor_buffer.hpp>
#include <nadir/render/braille.hpp>
#include <nadir/core/terminal.hpp>
#include <cmath>
#include <algorithm>
#include <sstream>

namespace nadir::render {

Presenter::Presenter(int width, int height)
    : w_(width), h_(height),
      virtual_w_(width * 2), virtual_h_(height * 4),
      current_(width * height),
      previous_(width * height),
      hud_current_(width * 4),
      hud_previous_(width * 4)
{
    // Pre-allocate HUD text buffer
    hud_text_.reserve(width * 4);
}

void Presenter::resize(int width, int height) {
    w_ = width;
    h_ = height;
    virtual_w_ = width * 2;
    virtual_h_ = height * 4;
    current_.assign(static_cast<std::size_t>(width * height), TerminalCell{});
    previous_.assign(static_cast<std::size_t>(width * height), TerminalCell{});
    hud_current_.assign(static_cast<std::size_t>(width * 4), TerminalCell{});
    hud_previous_.assign(static_cast<std::size_t>(width * 4), TerminalCell{});
}

void Presenter::render(const PhosphorBuffer& phosphor, const HUDState& hud) {
    // Render phosphor buffer to cells
    render_phosphor_to_cells(phosphor);

    // Render HUD
    render_hud(hud);

    // `previous_` intentionally remains the last frame presented until
    // write_diff() has emitted this frame.
}

void Presenter::render_phosphor_to_cells(const PhosphorBuffer& phosphor) {
    std::fill(current_.begin(), current_.end(), TerminalCell{});
    const int scene_rows = std::max(0, h_ - 4);
    for (int cy = 0; cy < scene_rows; ++cy) {
        for (int cx = 0; cx < w_; ++cx) {
            // Sample 2x4 subpixels from phosphor buffer
            float samples[2][4] = {};
            for (int sy = 0; sy < 4; ++sy) {
                for (int sx = 0; sx < 2; ++sx) {
                    const int px = cx * 2 + sx;
                    const int py = cy * 4 + sy;
                    if (px < phosphor.width() && py < phosphor.height()) {
                        samples[sx][sy] = phosphor.get(px, py);
                    }
                }
            }

            const char32_t glyph = encode_braille_cell(samples);
            const auto cell = phosphor.get_cell(cx * 2, cy * 4);

            const int idx = cy * w_ + cx;
            current_[idx].glyph = glyph;
            current_[idx].r = cell.r;
            current_[idx].g = cell.g;
            current_[idx].b = cell.b;
            current_[idx].attributes = cell.attributes;
        }
    }
}

void Presenter::render_hud(const HUDState& hud) {
    std::ostringstream oss;
    oss << "NADIR ORBITAL CONTROL  ";
    oss << "UTC " << hud.utc << "  ";
    oss << "FPS " << hud.fps << "  ";
    oss << "MODE " << hud.frame_mode << "  ";
    oss << "CAM " << hud.camera_mode << "  ";
    if (hud.trails) oss << "TRAILS ";
    if (hud.grid) oss << "GRID ";
    if (hud.stars) oss << "STARS ";
    oss << "ENT " << hud.entities;

    hud_text_ = oss.str();

    // Render HUD text to cells (bottom 4 rows)
    const int hud_rows = 4;
    for (int row = 0; row < hud_rows; ++row) {
        for (int col = 0; col < w_; ++col) {
            const int idx = row * w_ + col;
            char32_t glyph = U' ';
            if (row == 0 && col < static_cast<int>(hud_text_.size())) {
                glyph = hud_text_[col];
            }

            hud_current_[idx].glyph = glyph;
            hud_current_[idx].r = 0x00;
            hud_current_[idx].g = 0xAA;
            hud_current_[idx].b = 0x00;
            hud_current_[idx].attributes = 0;
        }
    }
}

void Presenter::compute_diff() {
    // Kept as a non-mutating seam for a future run-based diff builder.
}

void Presenter::present(nadir::TerminalSession& terminal) {
    write_diff(terminal);
}

void Presenter::write_diff(nadir::TerminalSession& terminal) {
    // Write main frame
    for (int cy = 0; cy < h_ - 4; ++cy) {
        bool row_changed = false;
        int first_changed = -1;
        int last_changed = -1;

        for (int cx = 0; cx < w_; ++cx) {
            const int idx = cy * w_ + cx;
            if (current_[idx].glyph != previous_[idx].glyph ||
                current_[idx].r != previous_[idx].r ||
                current_[idx].g != previous_[idx].g ||
                current_[idx].b != previous_[idx].b) {
                if (first_changed == -1) first_changed = cx;
                last_changed = cx;
                row_changed = true;
            }
        }

        if (row_changed) {
            terminal.write("\x1b[" + std::to_string(cy + 1) + ";" + std::to_string(first_changed + 1) + "H");

            for (int cx = first_changed; cx <= last_changed; ++cx) {
                const int idx = cy * w_ + cx;
                const auto& cell = current_[idx];

                // Write color if changed
                if (cell.r != 0 || cell.g != 0 || cell.b != 0) {
                    terminal.write("\x1b[38;2;" + std::to_string(cell.r) + ";" +
                                   std::to_string(cell.g) + ";" + std::to_string(cell.b) + "m");
                }

                // Write glyph (UTF-8)
                if (cell.glyph <= 0x7F) {
                    terminal.write(std::string(1, static_cast<char>(cell.glyph)));
                } else if (cell.glyph <= 0x7FF) {
                    char buf[2];
                    buf[0] = 0xC0 | (cell.glyph >> 6);
                    buf[1] = 0x80 | (cell.glyph & 0x3F);
                    terminal.write(std::string(buf, 2));
                } else if (cell.glyph <= 0xFFFF) {
                    char buf[3];
                    buf[0] = 0xE0 | (cell.glyph >> 12);
                    buf[1] = 0x80 | ((cell.glyph >> 6) & 0x3F);
                    buf[2] = 0x80 | (cell.glyph & 0x3F);
                    terminal.write(std::string(buf, 3));
                } else {
                    char buf[4];
                    buf[0] = 0xF0 | (cell.glyph >> 18);
                    buf[1] = 0x80 | ((cell.glyph >> 12) & 0x3F);
                    buf[2] = 0x80 | ((cell.glyph >> 6) & 0x3F);
                    buf[3] = 0x80 | (cell.glyph & 0x3F);
                    terminal.write(std::string(buf, 4));
                }
            }

            // Reset color
            terminal.write("\x1b[0m");
        }
    }

    // Write HUD (last 4 rows)
    int hud_start_row = h_ - 4;
    for (int row = 0; row < 4; ++row) {
        bool row_changed = false;
        int first_changed = -1;
        int last_changed = -1;

        for (int cx = 0; cx < w_; ++cx) {
            const int idx = row * w_ + cx;
            if (hud_current_[idx].glyph != hud_previous_[idx].glyph ||
                hud_current_[idx].r != hud_previous_[idx].r ||
                hud_current_[idx].g != hud_previous_[idx].g ||
                hud_current_[idx].b != hud_previous_[idx].b) {
                if (first_changed == -1) first_changed = cx;
                last_changed = cx;
                row_changed = true;
            }
        }

        if (row_changed) {
            terminal.write("\x1b[" + std::to_string(hud_start_row + row + 1) + ";" +
                          std::to_string(first_changed + 1) + "H");

            for (int cx = first_changed; cx <= last_changed; ++cx) {
                const int idx = row * w_ + cx;
                const auto& cell = hud_current_[idx];

                if (cell.r != 0 || cell.g != 0 || cell.b != 0) {
                    terminal.write("\x1b[38;2;" + std::to_string(cell.r) + ";" +
                                   std::to_string(cell.g) + ";" + std::to_string(cell.b) + "m");
                }

                if (cell.glyph <= 0x7F) {
                    terminal.write(std::string(1, static_cast<char>(cell.glyph)));
                } else if (cell.glyph <= 0x7FF) {
                    char buf[2];
                    buf[0] = 0xC0 | (cell.glyph >> 6);
                    buf[1] = 0x80 | (cell.glyph & 0x3F);
                    terminal.write(std::string(buf, 2));
                } else if (cell.glyph <= 0xFFFF) {
                    char buf[3];
                    buf[0] = 0xE0 | (cell.glyph >> 12);
                    buf[1] = 0x80 | ((cell.glyph >> 6) & 0x3F);
                    buf[2] = 0x80 | (cell.glyph & 0x3F);
                    terminal.write(std::string(buf, 3));
                } else {
                    char buf[4];
                    buf[0] = 0xF0 | (cell.glyph >> 18);
                    buf[1] = 0x80 | ((cell.glyph >> 12) & 0x3F);
                    buf[2] = 0x80 | ((cell.glyph >> 6) & 0x3F);
                    buf[3] = 0x80 | (cell.glyph & 0x3F);
                    terminal.write(std::string(buf, 4));
                }
            }
            terminal.write("\x1b[0m");
        }
    }

    previous_ = current_;
    hud_previous_ = hud_current_;
}

}
