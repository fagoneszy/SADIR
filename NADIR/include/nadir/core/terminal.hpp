#pragma once
#include <string>
#include <cstdint>
#include <functional>

namespace nadir {

enum class ColorMode {
    Mono,
    Ansi16,
    Ansi256,
    TrueColor
};

struct TerminalSize {
    int columns{120};
    int rows{40};
};

struct TerminalCapabilities {
    bool vt_output{false};
    bool alternate_screen{false};
    bool cursor_control{false};
    bool input_polling{false};

    ColorMode color{ColorMode::Mono};
};

struct InputState {
    bool quit{false};
    bool up{false};
    bool down{false};
    bool left{false};
    bool right{false};
    bool zoom_in{false};
    bool zoom_out{false};
    bool toggle_grid{false};
    bool toggle_hud{false};
    bool toggle_trails{false};
    bool toggle_stars{false};
    bool paused{false};
    bool reset_camera{false};
    bool reload{false};
    bool resized{false};
};

class Terminal {
public:
    static TerminalSize size();
    static void clear();
    static void home();
    static void hide_cursor();
    static void show_cursor();
    static void write(const std::string& text);

    static TerminalCapabilities capabilities();
    static bool enter_vt_mode();
    static void leave_vt_mode();
    static bool enter_alternate_screen();
    static void leave_alternate_screen();
    static InputState poll_input();
    static bool escape_pressed();
};

class TerminalSession {
public:
    TerminalSession();
    ~TerminalSession();

    TerminalSession(const TerminalSession&) = delete;
    TerminalSession& operator=(const TerminalSession&) = delete;

    bool valid() const noexcept;

    TerminalCapabilities capabilities() const noexcept;

    TerminalSize size() const noexcept;

    InputState poll_input();

    void home() const;
    void clear() const;
    void hide_cursor() const;
    void show_cursor() const;
    void write(const std::string& text) const;

private:
    bool valid_{false};
    TerminalCapabilities caps_;
    TerminalSize size_;
};

}
