#pragma once

#include <ncurses.h>
#include <string>
#include <map>

namespace nc
{
    static constexpr int CTRL_G = static_cast<int>('g') & (0x1f);
    static constexpr int CTRL_C = static_cast<int>('c') & (0x1f);
    static constexpr int CTRL_X = static_cast<int>('x') & (0x1f);
    static constexpr int CTRL_Q = static_cast<int>('q') & (0x1f);

    void init();

    void cleanup();

    int ctrl(char c);

    bool is_backspace(int c);

    int rows();

    int cols();

    class Window
    {
    public:
        Window();
        Window(int win_height, int win_width, int win_row, int win_col, std::string fill = "");
        ~Window();

        Window(const Window &window) = default;
        Window &operator=(const Window &window) = default;

        Window(Window &&window) = default;
        Window &operator=(Window &&window) = default;

        void move_cursor(int row, int col);
        void display_text(std::string text);
        int get_input();

        void reload();
        void resize(int new_height, int new_width);
        void reposition(int new_row, int new_col);

        int get_width();
        int get_height();

        void set_vertical_expansion(bool value);
        void set_horizontal_expansion(bool value);
        bool expands_vertically();
        bool expands_horizontally();

    protected:
        WINDOW *window_ptr; // Not a unique_ptr because WINDOW* has a special delete function

        int width;
        int height;
        int row;
        int col;
        bool expand_vertically;
        bool expand_horizontally;

        std::string current_text;
        std::string fill_pattern;
    };

    class Layout
    {
    public:
        Layout();
        Layout(int height, int width);

        /* Returns Layout& to allow chaining, i.e. layout.add(win1).add(win2) */
        Layout &add(Window &window, int layer_y, int layer_x);
        void refresh();

    protected:
        bool fullscreen = false;

        int max_width;
        int max_height;

        int total_width;
        int total_height;

        // kinda cursed, should refactor this at some point.
        // Outer map is keyed on y position (rows) of windows, inner map is keyed on x position
        // (column) of windows
        std::map<int, std::map<int, std::reference_wrapper<Window>>> y_layers;
    };
} /* namespace nc */

// TODO:
//  - make layout detect if expanding layer has widget that would be taller than the expansion
//  - make window width/height settable
//  - change std::ref_wrap in y_layers. having to use .get() everywhere is ugly