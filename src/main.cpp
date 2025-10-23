#include <cstdlib>
#include <ncurses.h>

#include <iostream>
#include <string>

#include "ncurses_utilities.h"

enum class Mode
{
    EDITING,
    GOTO,
    CONFIRMATION
};

int main(int argc, char *argv[])
{
    nc::init();

    int ch;
    bool refresh_triggered = false;
    Mode current_mode = Mode::EDITING;

    std::string text = "";
    int cursor_x = 0;
    int cursor_y = 0;

    nc::Window title_bar(1, nc::cols(), 0, 0);
    nc::Window editor(nc::rows() - 2, nc::cols(), 1, 0, "~");
    nc::Window command_bar(1, nc::cols(), nc::rows() - 1, 0);

    title_bar.display_text("title bar");
    editor.display_text(text);
    command_bar.display_text("command bar");

    editor.move_cursor(cursor_y, cursor_x);

    nc::Layout layout;
    layout.add(title_bar, 1).add(editor).add(command_bar, 1);
    layout.refresh();

    nc::Window focused_window = editor;

    while (true)
    {
        if (current_mode == Mode::EDITING)
            focused_window = editor;
        else if (current_mode == Mode::GOTO || current_mode == Mode::CONFIRMATION)
            focused_window = command_bar;

        ch = focused_window.get_input();

        switch (ch)
        {
        case ERR:
            continue;
        case KEY_RESIZE:
            refresh_triggered = true;
            break;
        case KEY_BACKSPACE:
        case 127:
        case '\b':
            if (!text.empty())
            {
                text.pop_back();

                if (cursor_x == 0)
                    cursor_y = std::max(cursor_y - 1, 0);

                cursor_x = std::max(cursor_x - 1, 0);
                focused_window.display_text(text);
            }
            break;
        case KEY_DOWN:
            cursor_y = std::min(cursor_y + 1, focused_window.get_height());
            break;
        case KEY_UP:
            cursor_y = std::max(cursor_y - 1, 0);
            break;
        case KEY_LEFT:
            if (cursor_x == 0)
                cursor_y = std::max(cursor_y - 1, 0);

            cursor_x = std::max(cursor_x - 1, 0);
            break;
        case KEY_RIGHT:
            cursor_x = std::min(cursor_x + 1, focused_window.get_width());
            break;
        case nc::CTRL_C:
        case nc::CTRL_X:
        case nc::CTRL_Q:
            nc::cleanup();
            std::exit(0);
            break;
        case nc::CTRL_G:
            current_mode = current_mode == Mode::EDITING ? Mode::GOTO : Mode::EDITING;
            break;
        case '\n':
            cursor_y = std::min(cursor_y + 1, focused_window.get_height());
            cursor_x = 0;

            text += static_cast<char>(ch);
            focused_window.display_text(text);
            break;
        default:
            cursor_x++;
            text += static_cast<char>(ch);
            focused_window.display_text(text);
            break;
        };

        focused_window.move_cursor(cursor_y, cursor_x);

        if (refresh_triggered)
        {
            layout.refresh();
        }
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