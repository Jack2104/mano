#include <cstdlib>
#include <ncurses.h>

#include <iostream>
#include <string>

#include "ncurses_utilities.h"

int main(int argc, char *argv[])
{
    nc::init();

    int ch;
    bool refresh_triggered = false;

    std::string text = "";
    int cursor_x = 0;
    int cursor_y = 0;

    nc::Window title_bar(1, nc::cols(), 0, 0);
    nc::Window editor(nc::rows() - 2, nc::cols(), 1, 0, "~");
    nc::Window command_bar(1, nc::cols(), nc::rows() - 1, 0);

    title_bar.display_text("title bar");
    command_bar.display_text("command bar");

    nc::Layout layout;
    layout.add(title_bar, 1).add(editor).add(command_bar, 1);
    layout.refresh();

    while (true)
    {
        ch = editor.get_input();

        // switch (ch)
        // {
        // case KEY_RESIZE:
        //     layout.refresh();
        //     break;
        // };

        if (ch == ERR)
        {
            continue;
        }
        else if (ch == KEY_RESIZE)
        {
            layout.refresh();
        }
        else if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b')
        {
            if (!text.empty())
            {
                text.pop_back();

                if (cursor_x == 0)
                    cursor_y = std::max(cursor_y - 1, 0);

                cursor_x = std::max(cursor_x - 1, 0);
                editor.display_text(text);
            }
        }
        else if (ch == KEY_DOWN)
        {
            cursor_y = std::min(cursor_y + 1, editor.get_height());
        }
        else if (ch == KEY_UP)
        {
            cursor_y = std::max(cursor_y - 1, 0);
        }
        else if (ch == KEY_LEFT)
        {
            if (cursor_x == 0)
                cursor_y = std::max(cursor_y - 1, 0);

            cursor_x = std::max(cursor_x - 1, 0);
        }
        else if (ch == KEY_RIGHT)
        {
            cursor_x = std::min(cursor_x + 1, editor.get_width());
        }
        else if (ch == nc::ctrl('c') || ch == nc::ctrl('x') || ch == nc::ctrl('q'))
        {
            break;
        }
        else if (ch == nc::ctrl('g'))
        {
        }
        else if (ch == '\n')
        {
            cursor_y = std::min(cursor_y + 1, editor.get_height());
            cursor_x = 0;

            text += static_cast<char>(ch);
            editor.display_text(text);
        }
        else
        {
            cursor_x++;
            text += static_cast<char>(ch);
            editor.display_text(text);
        }

        editor.move_cursor(cursor_y, cursor_x);
    }

    nc::cleanup();
    std::exit(0);

    // while true

    // handle keyboard shortcut

    // handle any other input
    // - pass to LSP

    // save (or update server)
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