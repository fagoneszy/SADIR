#include <nadir/core/terminal.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <poll.h>
#endif

namespace nadir {

namespace {

#ifdef _WIN32
HANDLE g_stdout_handle = INVALID_HANDLE_VALUE;
HANDLE g_stdin_handle = INVALID_HANDLE_VALUE;
DWORD g_original_console_mode = 0;
DWORD g_original_stdin_mode = 0;
bool g_vt_enabled = false;
bool g_alternate_screen = false;
CONSOLE_SCREEN_BUFFER_INFO g_original_sbi{};
#else
bool g_vt_enabled = false;
bool g_alternate_screen = false;
struct termios g_original_termios{};
int g_original_stdin_flags = 0;
#endif

bool detect_vt_support() {
#ifdef _WIN32
    g_stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (g_stdout_handle == INVALID_HANDLE_VALUE) return false;

    DWORD mode = 0;
    if (!GetConsoleMode(g_stdout_handle, &mode)) return false;

    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(g_stdout_handle, mode)) return false;

    g_original_console_mode = mode & ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    return true;
#else
    if (!isatty(STDOUT_FILENO)) return false;
    return true;
#endif
}

bool enable_vt_mode() {
#ifdef _WIN32
    if (!detect_vt_support()) return false;
    g_vt_enabled = true;
    return true;
#else
    if (!isatty(STDOUT_FILENO)) return false;
    g_vt_enabled = true;
    return true;
#endif
}

void disable_vt_mode() {
#ifdef _WIN32
    if (g_stdout_handle != INVALID_HANDLE_VALUE && g_vt_enabled) {
        SetConsoleMode(g_stdout_handle, g_original_console_mode);
    }
#else
    // VT mode doesn't need explicit disable on Unix
#endif
    g_vt_enabled = false;
}

bool enable_alternate_screen() {
    if (!g_vt_enabled) return false;

    // ESC[?1049h - Enter alternate screen buffer
    std::cout << "\x1b[?1049h" << std::flush;
    g_alternate_screen = true;
    return true;
}

void leave_alternate_screen() {
    if (!g_alternate_screen) return;

    // ESC[?1049l - Leave alternate screen buffer
    std::cout << "\x1b[?1049l" << std::flush;
    g_alternate_screen = false;
}

bool enable_input_polling() {
#ifdef _WIN32
    g_stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
    if (g_stdin_handle == INVALID_HANDLE_VALUE) return false;

    if (!GetConsoleMode(g_stdin_handle, &g_original_stdin_mode)) return false;

    DWORD mode = g_original_stdin_mode;
    mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    mode |= ENABLE_VIRTUAL_TERMINAL_INPUT | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
    if (!SetConsoleMode(g_stdin_handle, mode)) return false;

    return true;
#else
    if (!isatty(STDIN_FILENO)) return false;

    struct termios termios{};
    if (tcgetattr(STDIN_FILENO, &termios) != 0) return false;
    g_original_termios = termios;

    termios.c_lflag &= ~(ICANON | ECHO);
    termios.c_cc[VMIN] = 0;
    termios.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &termios) != 0) return false;

    return true;
#endif
}

void disable_input_polling() {
#ifdef _WIN32
    if (g_stdin_handle != INVALID_HANDLE_VALUE) {
        SetConsoleMode(g_stdin_handle, g_original_stdin_mode);
    }
#else
    if (isatty(STDIN_FILENO)) {
        tcsetattr(STDIN_FILENO, TCSANOW, &g_original_termios);
    }
#endif
}

bool check_escape_pressed() {
#ifdef _WIN32
    DWORD events = 0;
    if (!GetNumberOfConsoleInputEvents(g_stdin_handle, &events)) return false;
    if (events == 0) return false;

    std::vector<INPUT_RECORD> records(events);
    DWORD read = 0;
    if (!ReadConsoleInputW(g_stdin_handle, records.data(), static_cast<DWORD>(records.size()), &read)) return false;

    for (DWORD i = 0; i < read; ++i) {
        if (records[i].EventType == KEY_EVENT && records[i].Event.KeyEvent.bKeyDown) {
            if (records[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
                return true;
            }
        }
    }
    return false;
#else
    struct pollfd fds[1];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    if (poll(fds, 1, 0) <= 0) return false;

    char buf[16];
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
    if (n <= 0) return false;

    for (ssize_t i = 0; i < n; ++i) {
        if (buf[i] == 0x1b) { // ESC
            return true;
        }
    }
    return false;
#endif
}

InputState poll_input_state() {
    InputState state{};

    if (check_escape_pressed()) {
        state.quit = true;
    }

#ifdef _WIN32
    DWORD events = 0;
    if (!GetNumberOfConsoleInputEvents(g_stdin_handle, &events)) return state;
    if (events == 0) return state;

    std::vector<INPUT_RECORD> records(events);
    DWORD read = 0;
    if (!ReadConsoleInputW(g_stdin_handle, records.data(), static_cast<DWORD>(records.size()), &read)) return state;

    for (DWORD i = 0; i < read; ++i) {
        const auto& rec = records[i];
        if (rec.EventType == KEY_EVENT && rec.Event.KeyEvent.bKeyDown) {
            WORD vk = rec.Event.KeyEvent.wVirtualKeyCode;
            bool shift = (rec.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED) != 0;
            (void)shift;

            switch (vk) {
                case VK_UP:    state.up = true; break;
                case VK_DOWN:  state.down = true; break;
                case VK_LEFT:  state.left = true; break;
                case VK_RIGHT: state.right = true; break;
                case VK_ADD:
                case VK_OEM_PLUS: state.zoom_in = true; break;
                case VK_SUBTRACT:
                case VK_OEM_MINUS: state.zoom_out = true; break;
                case 'G': state.toggle_grid = true; break;
                case 'H': state.toggle_hud = true; break;
                case 'T': state.toggle_trails = true; break;
                case 'S': state.toggle_stars = true; break;
                case 'P': state.paused = true; break;
                case 'R': state.reset_camera = true; break;
                case VK_F5: state.reload = true; break;
            }
        }
    }
#else
    struct pollfd fds[1];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    if (poll(fds, 1, 0) <= 0) return state;

    char buf[32];
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
    if (n <= 0) return state;

    for (ssize_t i = 0; i < n; ++i) {
        char c = buf[i];
        if (c == 0x1b) {
            if (i + 2 < n && buf[i + 1] == '[') {
                char seq = buf[i + 2];
                if (seq == 'A') state.up = true;
                else if (seq == 'B') state.down = true;
                else if (seq == 'C') state.right = true;
                else if (seq == 'D') state.left = true;
                i += 2;
            }
        } else if (c == 'q' || c == 'Q') state.quit = true;
        else if (c == 'w' || c == 'W') state.up = true;
        else if (c == 's' || c == 'S') state.down = true;
        else if (c == 'a' || c == 'A') state.left = true;
        else if (c == 'd' || c == 'D') state.right = true;
        else if (c == '+' || c == '=') state.zoom_in = true;
        else if (c == '-' || c == '_') state.zoom_out = true;
        else if (c == 'g' || c == 'G') state.toggle_grid = true;
        else if (c == 'h' || c == 'H') state.toggle_hud = true;
        else if (c == 't' || c == 'T') state.toggle_trails = true;
        else if (c == ' ') state.toggle_stars = true;
        else if (c == 'p' || c == 'P') state.paused = true;
        else if (c == 'r' || c == 'R') state.reset_camera = true;
    }
#endif

    return state;
}

} // namespace

TerminalCapabilities Terminal::capabilities() {
    TerminalCapabilities caps{};
    caps.vt_output = g_vt_enabled;
    caps.alternate_screen = g_alternate_screen;
    caps.cursor_control = g_vt_enabled;
    caps.input_polling = g_vt_enabled;

    // Detect color support
#ifdef _WIN32
    caps.color = ColorMode::TrueColor;
#else
    const char* term = getenv("TERM");
    if (term && (std::string(term).find("256color") != std::string::npos ||
                 std::string(term).find("truecolor") != std::string::npos)) {
        caps.color = ColorMode::TrueColor;
    } else if (term && std::string(term).find("color") != std::string::npos) {
        caps.color = ColorMode::Ansi256;
    } else {
        caps.color = ColorMode::Ansi16;
    }
#endif
    return caps;
}

bool Terminal::enter_vt_mode() {
    return enable_vt_mode();
}

void Terminal::leave_vt_mode() {
    disable_vt_mode();
}

bool Terminal::enter_alternate_screen() {
    return enable_alternate_screen();
}

void Terminal::leave_alternate_screen() {
    ::nadir::leave_alternate_screen();
}

InputState Terminal::poll_input() {
    return poll_input_state();
}

bool Terminal::escape_pressed() {
    return check_escape_pressed();
}

TerminalSession::TerminalSession() {
    if (!enable_vt_mode()) {
        valid_ = false;
        return;
    }

    if (!enable_input_polling()) {
        disable_vt_mode();
        valid_ = false;
        return;
    }

    if (!enable_alternate_screen()) {
        disable_input_polling();
        disable_vt_mode();
        valid_ = false;
        return;
    }

    std::cout << "\x1b[?25l" << std::flush; // hide cursor

    caps_ = capabilities();
    size_ = size();
    valid_ = true;
}

TerminalSession::~TerminalSession() {
    if (!valid_) return;

    std::cout << "\x1b[0m" << std::flush;    // reset SGR
    std::cout << "\x1b[?25h" << std::flush;  // show cursor
    leave_alternate_screen();
    disable_input_polling();
    disable_vt_mode();
}

bool TerminalSession::valid() const noexcept {
    return valid_;
}

TerminalCapabilities TerminalSession::capabilities() const noexcept {
    return caps_;
}

TerminalSize TerminalSession::size() const noexcept {
    return size_;
}

InputState TerminalSession::poll_input() {
    return poll_input_state();
}

void TerminalSession::home() const {
    std::cout << "\x1b[H" << std::flush;
}

void TerminalSession::clear() const {
    std::cout << "\x1b[2J\x1b[H" << std::flush;
}

void TerminalSession::hide_cursor() const {
    std::cout << "\x1b[?25l" << std::flush;
}

void TerminalSession::show_cursor() const {
    std::cout << "\x1b[?25h" << std::flush;
}

void TerminalSession::write(const std::string& text) const {
    std::cout << text << std::flush;
}

TerminalSize Terminal::size() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) {
        return {info.srWindow.Right - info.srWindow.Left + 1, info.srWindow.Bottom - info.srWindow.Top + 1};
    }
#else
    winsize ws{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0 && ws.ws_row > 0) {
        return {static_cast<int>(ws.ws_col), static_cast<int>(ws.ws_row)};
    }
#endif
    return {120, 40};
}

void Terminal::clear() { write("\x1b[2J\x1b[H"); }
void Terminal::home() { write("\x1b[H"); }
void Terminal::hide_cursor() { write("\x1b[?25l"); }
void Terminal::show_cursor() { write("\x1b[?25h"); }
void Terminal::write(const std::string& text) { std::cout << text << std::flush; }

}