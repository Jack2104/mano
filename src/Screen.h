#pragma once

#include <string>
#include <vector>

#include <ncurses.h>

struct DisplayArea
{
    int width;
    int height;
};

class Screen
{
public:
    Screen(int max_height, int max_width);
    Screen(const Screen &screen) = delete;
    Screen(Screen &&screen) = delete;
    Screen &operator=(const Screen &screen) = delete;
    Screen &operator=(Screen &&screen) = delete;
    ~Screen();

    void set_display_text(std::string text);
    void set_title_bar_text(std::string text);
    void set_command_bar_text(std::string text);
    void set_cursor_pos(int row, int col);

    int get_editor_input();
    int get_command_bar_input();

    DisplayArea get_display_area();

protected:
    void render();

    int height;
    int width;
    DisplayArea display_area;

    // TODO: make RAII wrapper
    WINDOW *title_bar;
    WINDOW *editor;
    WINDOW *command_bar;
};

// RAII wrapper for windows - supports getting input, displaying text, placing cursor, etc
//     - maybe handle cursors, i.e. if text is too big for window.
//       could split by newline and keep track of cursor position for line/col inside class
//       and if it gets to edge, move rendering window over by 1
// Layout class - register windows (Layout(window1, window2)) and have Layout::resize()
//    - also supports expanding windows, etc.
// main gets input from window, if resize does layout.resize()
// maybe InputManager - register univeral callbacks that happen on keyboard shortcuts
