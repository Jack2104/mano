#include "ncpp/Window.h"

#include <algorithm>

namespace ncpp
{
    Window::Window() : Window(0, 0, 0, 0) {};

    Window::Window(int win_height, int win_width, int win_row, int win_col, std::string fill) : height(win_height), width(win_width), row(win_row), col(win_col), fill_pattern(fill)
    {
        window_ptr = newwin(height, width, row, col);
        keypad(window_ptr, true);
    }

    Window::~Window()
    {
        delwin(window_ptr);
    }

    void Window::move_cursor(const Cursor &cursor)
    {
        move_cursor(cursor.row, cursor.col);
    }

    void Window::move_cursor(int row, int col)
    {
        wmove(window_ptr, row, col);
    }

    void Window::display_text(std::string text)
    {
        current_text = text;
        std::string filled_text = text;

        if (fill_pattern != "")
        {
            auto newline_count = static_cast<int>(std::ranges::count(text, '\n'));
            int fill_count = std::max(height - newline_count - 1, 0);

            for (int i = 0; i < fill_count; i++)
            {
                filled_text += '\n' + fill_pattern;
            }
        }

        werase(window_ptr);
        wprintw(window_ptr, filled_text.c_str());
        reload();
    }

    int Window::get_input()
    {
        return wgetch(window_ptr);
    }

    void Window::reload()
    {
        wrefresh(window_ptr);
    }

    void Window::resize(int new_height, int new_width)
    {
        if (new_width == width && new_height == height)
            return;

        width = new_width;
        height = new_height;

        wresize(window_ptr, new_height, new_width);
        display_text(current_text);
    }

    void Window::reposition(int new_row, int new_col)
    {
        if (new_row == row && new_col == col)
            return;

        row = new_row;
        col = new_col;

        werase(window_ptr);
        mvwin(window_ptr, new_row, new_col);
        display_text(current_text);
    }

    int Window::get_width()
    {
        return width;
    }

    int Window::get_height()
    {
        return height;
    }

    void Window::set_vertical_expansion(bool value) { expand_vertically = value; }
    void Window::set_horizontal_expansion(bool value) { expand_horizontally = value; }

    bool Window::expands_vertically() { return expand_vertically; }
    bool Window::expands_horizontally() { return expand_horizontally; }

} /* namespace ncpp */