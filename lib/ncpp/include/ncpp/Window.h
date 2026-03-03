#pragma once

#include "ncpp/ncpp.h"

namespace ncpp
{
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

        void move_cursor(const Cursor &cursor);
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

        void set_preamble(std::string text);

    protected:
        WINDOW *window_ptr; // Not a unique_ptr because WINDOW* has a special delete function

        int width;
        int height;
        int row;
        int col;
        bool expand_vertically = false;
        bool expand_horizontally = true;

        std::string current_text = "";
        std::string fill_pattern = "";
        std::string preamble = "";
    };
}