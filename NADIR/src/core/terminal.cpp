#include <nadir/core/terminal.hpp>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace nadir {

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
    return {};
}

void Terminal::clear() { write("\x1b[2J\x1b[H"); }
void Terminal::home() { write("\x1b[H"); }
void Terminal::hide_cursor() { write("\x1b[?25l"); }
void Terminal::show_cursor() { write("\x1b[?25h"); }
void Terminal::write(const std::string& text) { std::cout << text << std::flush; }

}
