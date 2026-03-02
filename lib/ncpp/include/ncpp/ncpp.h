#pragma once

#include <ncurses.h>
#include <string>
#include <map>
#include <memory>

struct Cursor
{
    int row;
    int col;
};

namespace ncpp
{
    static constexpr int CTRL_G = static_cast<int>('g') & (0x1f);
    static constexpr int CTRL_C = static_cast<int>('c') & (0x1f);
    static constexpr int CTRL_X = static_cast<int>('x') & (0x1f);
    static constexpr int CTRL_Q = static_cast<int>('q') & (0x1f);
    static constexpr int CTRL_S = static_cast<int>('s') & (0x1f);

    void init();

    void cleanup();

    int ctrl(char c);

    bool is_backspace(int c);

    int rows();

    int cols();
} /* namespace ncpp */

// TODO:
//   make layout detect if expanding layer has widget that would be taller than the expansion
//  - make window width/height settable
//  + change std::ref_wrap in y_layers. having to use .get() everywhere is ugly
//  - add constexpr variables for all used ncurses keys. also make them characters