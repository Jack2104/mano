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

    Layout::Layout() : layout_height(nc::rows()), layout_width(nc::cols()), fullscreen(true) {};

    Layout::Layout(int height, int width) : layout_height(height), layout_width(width), fullscreen(false) {};

    Layout &Layout::add(std::shared_ptr<Window> window, int layer_y, int layer_x)
    {
        if (!y_layers.contains(layer_y))
        {
            std::map<int, std::shared_ptr<Window>> window_map;
            y_layers.insert({layer_y, window_map});
        }

        y_layers[layer_y].insert({layer_x, window});

        return *this;
    }

    void Layout::refresh()
    {
        if (fullscreen)
        {
            layout_height = nc::rows();
            layout_width = nc::cols();
        }

        /* Calculate the total height taken up by fixed-height (i.e. non-expanding) windows. */
        int total_height = 0;
        int expanding_y_layers = 0;

        for (auto y_layer : y_layers)
        {
            /* Find the window with the largest height in this layer. */
            int max_layer_height = 0;
            bool layer_has_expanding_item = false;

            for (auto x_layer : y_layer.second)
            {
                if (x_layer.second->expands_vertically())
                    layer_has_expanding_item = true;
                else
                    max_layer_height = std::max(max_layer_height, x_layer.second->get_height());
            }

            total_height += max_layer_height;

            if (max_layer_height == 0 && layer_has_expanding_item)
                expanding_y_layers++;
        }

        /* Divide the remaining height among all expanding y layers. */
        int remaining_height = layout_height - total_height;
        int height_per_expanding_layer = expanding_y_layers > 0 ? remaining_height / expanding_y_layers : 0;
        int curr_row = 0;

        for (auto y_layer : y_layers)
        {
            int max_layer_height = 0;

            /* Calculate the width of any expanding x layers. */
            int expanding_x_layers = 0;
            int total_width = 0;

            for (auto x_layer : y_layer.second)
            {
                if (x_layer.second->expands_horizontally())
                    expanding_x_layers++;
                else
                    total_width += x_layer.second->get_width();

                if (!x_layer.second->expands_vertically())
                    max_layer_height = std::max(max_layer_height, x_layer.second->get_height());
            }

            /* Divide the remaining width among all expanding x layers. */
            int remaining_width = layout_width - total_width;
            int width_per_expanding_layer = expanding_x_layers > 0 ? remaining_width / expanding_x_layers : 0;
            int curr_col = 0;

            for (auto x_layer : y_layer.second)
            {
                int new_height = x_layer.second->get_height();

                if (x_layer.second->expands_vertically())
                    new_height = max_layer_height == 0 ? height_per_expanding_layer : max_layer_height;

                int new_width = x_layer.second->expands_horizontally() ? width_per_expanding_layer : x_layer.second->get_width();

                x_layer.second->reposition(curr_row, curr_col);
                x_layer.second->resize(new_height, new_width);

                curr_col += new_width;
            }

            curr_row += (max_layer_height == 0 ? height_per_expanding_layer : max_layer_height);
        }
    }
} /* namespace nc */
