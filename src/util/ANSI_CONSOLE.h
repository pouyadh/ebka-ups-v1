#ifndef ANSI_CONSOLE_HPP
#define ANSI_CONSOLE_HPP

#include <cstdio>

namespace ansi {

// Cursor Control
inline void cursor_move_to(int row = 1, int col = 1) {
    // ANSI uses 1-based indexing for rows and columns
    std::printf("\x1b[%d;%dH", row, col);
}
inline void cursor_up(int lines = 1) {
    std::printf("\x1b[%dA", lines);
}
inline void cursor_down(int lines = 1) {
    std::printf("\x1b[%dB", lines);
}
inline void cursor_forward(int cols = 1) {
    std::printf("\x1b[%dC", cols);
}
inline void cursor_back(int cols = 1) {
    std::printf("\x1b[%dD", cols);
}
inline void cursor_save_position() {
    std::printf("\x1b7"); // or \x1b[s
}
inline void cursor_restore_position() {
    std::printf("\x1b8"); // or \x1b[u
}

// Screen Control
inline void clear_screen() {
    std::printf("\x1b[2J");
}
inline void clear_line() {
    std::printf("\x1b[2K");
}
inline void clear_line_from_cursor() {
    std::printf("\x1b[0K");
}
inline void clear_line_to_cursor() {
    std::printf("\x1b[1K");
}
inline void clear_screen_and_move_home() {
    clear_screen();
    cursor_move_to(1,1);
}

// Cursor Visibility
inline void cursor_hide() {
    std::printf("\x1b[?25l");
}
inline void cursor_show() {
    std::printf("\x1b[?25h");
}

// Text Attributes & Colors
namespace color {
    constexpr const char* RESET       = "\x1b[0m";

    // Foreground (text) colors
    constexpr const char* BLACK       = "\x1b[30m";
    constexpr const char* RED         = "\x1b[31m";
    constexpr const char* GREEN       = "\x1b[32m";
    constexpr const char* YELLOW      = "\x1b[33m";
    constexpr const char* BLUE        = "\x1b[34m";
    constexpr const char* MAGENTA     = "\x1b[35m";
    constexpr const char* CYAN        = "\x1b[36m";
    constexpr const char* WHITE       = "\x1b[37m";

    // Bright foreground colors
    constexpr const char* BRIGHT_BLACK   = "\x1b[90m";
    constexpr const char* BRIGHT_RED     = "\x1b[91m";
    constexpr const char* BRIGHT_GREEN   = "\x1b[92m";
    constexpr const char* BRIGHT_YELLOW  = "\x1b[93m";
    constexpr const char* BRIGHT_BLUE    = "\x1b[94m";
    constexpr const char* BRIGHT_MAGENTA = "\x1b[95m";
    constexpr const char* BRIGHT_CYAN    = "\x1b[96m";
    constexpr const char* BRIGHT_WHITE   = "\x1b[97m";

    // Background colors
    constexpr const char* BG_BLACK     = "\x1b[40m";
    constexpr const char* BG_RED       = "\x1b[41m";
    constexpr const char* BG_GREEN     = "\x1b[42m";
    constexpr const char* BG_YELLOW    = "\x1b[43m";
    constexpr const char* BG_BLUE      = "\x1b[44m";
    constexpr const char* BG_MAGENTA   = "\x1b[45m";
    constexpr const char* BG_CYAN      = "\x1b[46m";
    constexpr const char* BG_WHITE     = "\x1b[47m";

    // Bright background colors
    constexpr const char* BG_BRIGHT_BLACK   = "\x1b[100m";
    constexpr const char* BG_BRIGHT_RED     = "\x1b[101m";
    constexpr const char* BG_BRIGHT_GREEN   = "\x1b[102m";
    constexpr const char* BG_BRIGHT_YELLOW  = "\x1b[103m";
    constexpr const char* BG_BRIGHT_BLUE    = "\x1b[104m";
    constexpr const char* BG_BRIGHT_MAGENTA = "\x1b[105m";
    constexpr const char* BG_BRIGHT_CYAN    = "\x1b[106m";
    constexpr const char* BG_BRIGHT_WHITE   = "\x1b[107m";
}

// Text styles
namespace style {
    constexpr const char* BOLD      = "\x1b[1m";
    constexpr const char* DIM       = "\x1b[2m";
    constexpr const char* ITALIC    = "\x1b[3m";
    constexpr const char* UNDERLINE = "\x1b[4m";
    constexpr const char* BLINK     = "\x1b[5m";
    constexpr const char* REVERSE   = "\x1b[7m";
    constexpr const char* HIDDEN    = "\x1b[8m";

    constexpr const char* RESET_ALL = "\x1b[0m";
}

} // namespace ansi

#endif // ANSI_CONSOLE_HPP
