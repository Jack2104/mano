#include <cstdlib>
#include <ncurses.h>

#include <iostream>
#include <string>

#include "ncurses_utilities.h"
#include "TextBuffer.h"

enum class Mode
{
    EDITING,
    GOTO,
    CONFIRMATION
};

void set_line_numbers(int start_num, int end_num, std::shared_ptr<nc::Window> win)
{
    int final_num = std::min(end_num, win->get_height());

    /* Quick and easy way to find the maximum number of digits for the line numbers. */
    int max_digit_count = std::to_string(final_num).length();

    std::string line_numbers = "";

    for (int i = start_num; i <= final_num; i++)
    {
        std::string line_num_str = std::to_string(i);

        if (line_num_str.length() < max_digit_count)
            line_num_str.insert(0, max_digit_count - line_num_str.length(), ' ');

        if (i < final_num)
            line_num_str += '\n';

        line_numbers += line_num_str;
    }

    win->resize(win->get_height(), max_digit_count + 1);
    win->display_text(line_numbers);
}

int main(int argc, char *argv[])
{
    nc::init();

    int ch;
    bool refresh_triggered = false;
    Mode current_mode = Mode::EDITING;

    std::string text = "";
    TextBuffer document_text = TextBuffer();

    std::shared_ptr<Cursor> editor_cursor = std::make_shared<Cursor>(0, 0);
    std::shared_ptr<Cursor> command_cursor = std::make_shared<Cursor>(0, 0);
    std::shared_ptr<Cursor> current_cursor = editor_cursor;

    std::shared_ptr<nc::Window> title_bar = std::make_shared<nc::Window>(1, nc::cols(), 0, 0);
    std::shared_ptr<nc::Window> gutter = std::make_shared<nc::Window>(nc::rows() - 2, 5, 1, 0, "~");
    std::shared_ptr<nc::Window> editor = std::make_shared<nc::Window>(nc::rows() - 2, nc::cols() - 5, 1, 5);
    std::shared_ptr<nc::Window> command_bar = std::make_shared<nc::Window>(1, nc::cols(), nc::rows() - 1, 0);

    title_bar->display_text("title bar");
    editor->display_text(text);
    command_bar->display_text("command bar");

    gutter->set_horizontal_expansion(false);
    gutter->set_vertical_expansion(true);
    set_line_numbers(1, 1, gutter);

    editor->set_vertical_expansion(true);
    editor->move_cursor(editor_cursor->row, editor_cursor->col);

    nc::Layout layout;
    layout.add(title_bar, 0, 0).add(gutter, 1, 0).add(editor, 1, 1).add(command_bar, 2, 0);
    layout.refresh();

    std::shared_ptr<nc::Window> focused_window = editor;

    int line_count = 1;

    while (true)
    {
        ch = focused_window->get_input();

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
            document_text.pop();

            if (!document_text.empty())
            {
                focused_window->display_text(document_text.get_text());
            }

            if (!text.empty())
            {
                text.pop_back();

                if (current_cursor->col == 0)
                {
                    line_count = std::max(line_count - 1, 1);
                    set_line_numbers(1, line_count, gutter);

                    current_cursor->row = std::max(current_cursor->row - 1, 0);
                }

                current_cursor->col = std::max(current_cursor->col - 1, 0);
                // focused_window->display_text(document_text.get_text());
            }
            break;
        case KEY_DOWN:
            current_cursor->row = std::min(current_cursor->row + 1, line_count - 1);
            break;
        case KEY_UP:
            current_cursor->row = std::max(current_cursor->row - 1, 0);
            break;
        case KEY_LEFT:
            if (current_cursor->col == 0)
                current_cursor->row = std::max(current_cursor->row - 1, 0);

            current_cursor->col = std::max(current_cursor->col - 1, 0);
            break;
        case KEY_RIGHT:
            current_cursor->col = std::min(current_cursor->col + 1, focused_window->get_width() - 1);
            break;
        case nc::CTRL_C:
        case nc::CTRL_X:
        case nc::CTRL_Q:
            nc::cleanup();
            std::exit(0);
            break;
        case nc::CTRL_G:
            if (current_mode == Mode::GOTO)
            {
                current_mode = Mode::EDITING;
                focused_window = editor;
                current_cursor = editor_cursor;
                focused_window->move_cursor(current_cursor->row, current_cursor->col);
            }
            else if (current_mode == Mode::EDITING)
            {
                current_mode = Mode::GOTO;
                focused_window = command_bar;
                current_cursor = command_cursor;
                current_cursor->col = 0;
                current_cursor->row = 0;
                focused_window->move_cursor(0, 0);
            }
            break;
        case '\n':
            line_count++;
            set_line_numbers(1, line_count, gutter);

            current_cursor->row = std::min(current_cursor->row + 1, line_count - 1);
            current_cursor->col = 0;

            text += static_cast<char>(ch);
            document_text.insert(static_cast<char>(ch));

            focused_window->display_text(document_text.get_text());
            break;
        default:
            current_cursor->col++;

            text += static_cast<char>(ch);
            document_text.insert(static_cast<char>(ch));

            focused_window->display_text(document_text.get_text());
            break;
        };

        if (refresh_triggered)
        {
            layout.refresh();
        }

        if (current_mode == Mode::EDITING)
            command_bar->display_text(std::to_string(current_cursor->row + 1) + ":" + std::to_string(current_cursor->col + 1));

        focused_window->move_cursor(current_cursor->row, current_cursor->col);
    }

    nc::cleanup();
    std::exit(0);

    // while true

    // handle keyboard shortcut

    // handle any other input
    // - pass to LSP

    // save (or update server)
}

// TODO:
//  + fix cursor bug
//  + add line numbers
//  + show current cursor position in command bar
//  - refactor state machine
//      - potentially move handling of arrow keys into separate function
//      - think about whether alternate modes will handle their own input, or if they'll all
//        handle input in the main loop
//  - add ability for mouse click to move cursor
//  - work on gap buffer
//  - add proper cursor movement + scrolling
//  - add GOTO functionality
//  - add selection
//  - add load/save

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