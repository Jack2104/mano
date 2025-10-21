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
        Window(int win_height, int win_width, int win_row, int win_col);
        ~Window();

        // Non-copyable. Windows are unique.
        Window(const Window &window) = delete;
        Window &operator=(const Window &window) = delete;

        // Allow moves because it tranfers ownership, and window stays unique.
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

    protected:
        WINDOW *window_ptr; // Not a unique_ptr because WINDOW* has a special delete function

        int width;
        int height;
        int row;
        int col;

        std::string current_text;
    };

    class Layout
    {
    public:
        Layout();
        Layout(int height, int width);

        /* Returns Layout& to allow chaining, i.e. layout.add(win1).add(win2) */
        Layout &add(Window &window);
        Layout &add(Window &window, int height, bool expanding = false);
        void refresh();

    protected:
        int max_width;
        int max_height;

        int total_width;
        int total_height;

        struct WindowInfo
        {
            Window &window;
            int height;
            bool expanding;
        };

        std::vector<WindowInfo> window_details;
    };
} /* namespace nc */
