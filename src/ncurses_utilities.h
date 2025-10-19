#pragma once

#include <ncurses.h>
#include <string>
#include <vector>

namespace nc
{
    void init();

    int rows();

    int cols();

    class Window
    {
    public:
        Window();
        Window(int height, int width, int row, int col);
        ~Window();

        // Non-copyable. Windows are unique.
        Window(const Window &window) = delete;
        Window &operator=(const Window &window) = delete;

        // Allow moves because it tranfers ownership, and window stays unique.
        Window(Window &&window) = default;
        Window &operator=(Window &&window) = default;

        void set_cursor_position(int row, int col);
        void display_text(std::string text);
        int get_input();

        void reload();
        void resize(int new_width, int new_height);

    protected:
        WINDOW *window_ptr;

        int width;
        int height;
        int row;
        int col;
    };

    class Layout
    {
    public:
    protected:
    };
} /* namespace nc */
