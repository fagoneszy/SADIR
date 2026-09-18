#pragma once
#include <string>

namespace nadir {

struct TerminalSize {
    int columns{120};
    int rows{40};
};

class Terminal {
public:
    static TerminalSize size();
    static void clear();
    static void home();
    static void hide_cursor();
    static void show_cursor();
    static void write(const std::string& text);
};

}
