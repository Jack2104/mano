#include "ncurses_utilities.h"

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

    Window::Window(int height, int width, int row, int col)
    {
        window_ptr = newwin(height, width, row, col);
        keypad(window_ptr, true);
    }

    Window::~Window()
    {
        delwin(window_ptr);
    }

    void Window::set_cursor_position(int row, int col)
    {
        wmove(window_ptr, row, col);
    }

    void Window::display_text(std::string text)
    {
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

    void Window::resize(int new_width, int new_height)
    {
        if (new_width == width && new_height == height)
            return;

        wresize(window_ptr, new_width, new_height);
    }
} /* namespace nc */