#include "ncurses_utilities.h"

#include <algorithm>

namespace nc
{
    void init()
    {
        initscr();
        noecho();
        keypad(stdscr, true);
        raw();
    }

    int rows()
    {
        return LINES;
    }

    int cols()
    {
        return COLS;
    }

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

    void Window::move_cursor(int row, int col)
    {
        wmove(window_ptr, row, col);
    }

    void Window::display_text(std::string text)
    {
        current_text = text;

        if (fill_pattern != "")
        {
            auto newline_count = static_cast<int>(std::ranges::count(current_text, '\n'));
            int fill_count = std::max(width - newline_count, 0);

            for (int i = 0; i < fill_count; i++)
            {
                current_text += fill_pattern + '\n';
            }
        }

        werase(window_ptr);
        wprintw(window_ptr, text.c_str());
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
        reload();
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

    Layout::Layout() : max_height(nc::rows()), max_width(nc::cols()), fullscreen(true) {};

    Layout::Layout(int height, int width) : max_height(height), max_width(width), fullscreen(false) {};

    Layout &Layout::add(Window &window)
    {
        return add(window, 0, true);
    }

    Layout &Layout::add(Window &window, int height, bool expanding)
    {
        window_details.push_back({window, height, expanding});

        return *this;
    }

    void Layout::refresh()
    {
        if (fullscreen)
        {
            max_height = nc::rows();
            max_width = nc::cols();
        }

        /* Calculate the total height taken up by fixed-height (i.e. non-expanding) windows. */
        int total_height = 0;
        int expanding_window_count = 0;

        for (WindowInfo &win_info : window_details)
        {
            if (!win_info.expanding)
                total_height += win_info.height;
            else
                expanding_window_count++;
        }

        /* Divide the remaining height among all expanding windows. */
        int remaining_height = max_height - total_height;
        int height_per_expanding_window = remaining_height / expanding_window_count;
        int curr_row = 0;

        for (WindowInfo &win_info : window_details)
        {
            if (win_info.expanding)
            {
                win_info.height = height_per_expanding_window;
                total_height += win_info.height;
            }

            win_info.window.reposition(curr_row, 0);
            win_info.window.resize(win_info.height, win_info.window.get_width());

            curr_row += win_info.height;
        }
    }
} /* namespace nc */
