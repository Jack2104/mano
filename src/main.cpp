#include <cstdlib>
#include <ncurses.h>

#include <iostream>
#include <string>

#include "Screen.h"
#include "ncurses_utilities.h"

void setup()
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

inline int ctrl(char c)
{
    return static_cast<int>(c) & (0x1f);
}

void handle_keyboard_shortcuts(char input)
{
}

int main(int argc, char *argv[])
{
    setup();

    // printw("hello world");
    // refresh();

    int ch;
    bool refresh_triggered = false;

    std::string text = "";
    int cursor_x = 0;
    int cursor_y = 0;

    // Screen screen(getmaxy(stdscr), getmaxx(stdscr));
    // screen.set_title_bar_text("title bar");
    // screen.set_display_text("display area");
    // screen.set_command_bar_text("command bar");

    nc::Window title_bar(1, nc::cols(), 0, 0);
    nc::Window editor(nc::rows() - 2, nc::cols(), 1, 0);
    nc::Window command_bar(1, nc::cols(), nc::rows() - 1, 0);

    while (true)
    {
        ch = editor.get_input();

        if (ch == ERR)
        {
            continue;
        }
        else if (ch == KEY_RESIZE)
        {
            // refresh();
        }
        else if (ch == KEY_BACKSPACE)
        {
            cursor_x = std::max(cursor_x - 1, 0);
        }
        else if (ch == KEY_DOWN)
        {
            cursor_y = std::min(cursor_y + 1, getmaxy(stdscr));
        }
        else if (ch == KEY_UP)
        {
            cursor_y = std::max(cursor_y - 1, 0);
        }
        else if (ch == KEY_LEFT)
        {
            cursor_x = std::max(cursor_x - 1, 0);
        }
        else if (ch == KEY_RIGHT)
        {
            cursor_x = std::min(cursor_x + 1, getmaxx(stdscr));
        }
        else if (ch == ctrl('c') || ch == ctrl('x') || ch == ctrl('q'))
        {
            break;
        }
        else if (ch == ctrl('g'))
        {
        }
        else
        {
            cursor_x++;
            // addch(static_cast<char>(ch));
            text += static_cast<char>(ch);
            editor.display_text(text);
            // erase();
            // screen.set_display_text(text);
            // printw(text.c_str());
            // refresh();
        }

        // printw("size: %d", getmaxx(stdscr));
        // move(cursor_y, cursor_x);

        if (refresh_triggered)
        {
            // refresh();
        }
    }

    cleanup();
    std::exit(0);

    // while true

    // handle keyboard shortcut

    // handle any other input
    // - pass to LSP

    // save (or update server)

    cleanup();
}

// Screen: handles printing and cursor
// e.g. screen.move_cursor(int x, int y, int max_x, int max_y) (will internally ensure x, y don't go beyone screen width/height)
//      (max_x = rope.max_line_length(line_num), max_y = rope.line_count)

// main: handles input/rope
// e.g. display_text = get_text(rope, line_lengths, etc.)
//      screen.display(display_text)

// will need some kind of way to convert between screen cursor pos and real pos
// e.g. screen_cursor_pos = screen.cursor_pos
//      display_text = rope.get_text(cursor_pos.y +/- (screen_cursor_pos || screen_cursor_pos - display_size))
// so will need to be able to get screen cursor position and display size from Screen