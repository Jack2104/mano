#include "ncpp/ncpp.h"

namespace ncpp
{
    void init()
    {
        initscr();
        noecho();
        keypad(stdscr, true);
        raw();
        // mousemask(ALL_MOUSE_EVENTS, NULL);
    }

    void cleanup()
    {
        endwin();
    }

    int ctrl(char c)
    {
        return static_cast<int>(c) & (0x1f);
    }

    bool is_backspace(int c)
    {
        return c == KEY_BACKSPACE || c == 127 || c == '\b';
    }

    int rows()
    {
        return LINES;
    }

    int cols()
    {
        return COLS;
    }
} /* namespace ncpp */