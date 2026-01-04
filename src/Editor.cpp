#include "Editor.h"

Editor::Editor()
{
    nc::init();

    editor_cursor = std::make_shared<Cursor>(0, 0);
    command_cursor = std::make_shared<Cursor>(0, 0);
    current_cursor = editor_cursor;

    title_bar = std::make_shared<nc::Window>(1, nc::cols(), 0, 0);
    gutter = std::make_shared<nc::Window>(nc::rows() - 2, 5, 1, 0, "~");
    editor = std::make_shared<nc::Window>(nc::rows() - 2, nc::cols() - 5, 1, 5);
    command_bar = std::make_shared<nc::Window>(1, nc::cols(), nc::rows() - 1, 0);

    title_bar->display_text("title bar");
    editor->display_text(document_text.get_text());
    command_bar->display_text("command bar");

    gutter->set_horizontal_expansion(false);
    gutter->set_vertical_expansion(true);
    set_line_numbers(1, 1);

    editor->set_vertical_expansion(true);
    editor->move_cursor(editor_cursor->row, editor_cursor->col);

    layout.add(title_bar, 0, 0).add(gutter, 1, 0).add(editor, 1, 1).add(command_bar, 2, 0);
    layout.refresh();

    focused_window = editor;
}

Editor::~Editor()
{
    nc::cleanup();
}

void Editor::set_line_numbers(int start_num, int end_num)
{
    int final_num = std::min(end_num, gutter->get_height());

    /* Quick and easy way to find the maximum number of digits for the line numbers. */
    int max_digit_count = std::to_string(final_num).length();

    std::string line_numbers = "";

    for (int i = start_num; i <= final_num; i++)
    {
        std::string line_num_str = std::to_string(i);

        if (line_num_str.length() < max_digit_count)
            line_num_str.insert(0, max_digit_count - line_num_str.length(), ' ');

        if (i < final_num)
            line_num_str += "\n";

        line_numbers += line_num_str;
    }

    gutter->resize(gutter->get_height(), max_digit_count + 1);
    gutter->display_text(line_numbers);
}

void Editor::start_state_machine()
{
    bool refresh_triggered = false;

    while (true)
    {
        int ch = focused_window->get_input();

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

            if (current_cursor->col == 0)
            {
                line_count = std::max(line_count - 1, 1);
                set_line_numbers(1, line_count);

                current_cursor->row = std::max(current_cursor->row - 1, 0);
            }

            current_cursor->col = std::max(current_cursor->col - 1, 0);
            focused_window->display_text(document_text.get_text());
            document_text.set_cursor_pos(current_cursor->row, current_cursor->col);
            break;
        case KEY_DOWN:
            current_cursor->row = std::min(current_cursor->row + 1, line_count - 1);
            document_text.set_cursor_pos(current_cursor->row, current_cursor->col);
            break;
        case KEY_UP:
            current_cursor->row = std::max(current_cursor->row - 1, 0);
            document_text.set_cursor_pos(current_cursor->row, current_cursor->col);
            break;
        case KEY_LEFT:
            if (current_cursor->col == 0)
                current_cursor->row = std::max(current_cursor->row - 1, 0);

            current_cursor->col = std::max(current_cursor->col - 1, 0);
            document_text.set_cursor_pos(current_cursor->row, current_cursor->col);
            break;
        case KEY_RIGHT:
            current_cursor->col = std::min(current_cursor->col + 1, focused_window->get_width() - 1);
            document_text.set_cursor_pos(current_cursor->row, current_cursor->col);
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
            set_line_numbers(1, line_count);

            current_cursor->row = std::min(current_cursor->row + 1, line_count - 1);
            current_cursor->col = 0;

            document_text.insert(static_cast<char>(ch));
            focused_window->display_text(document_text.get_text());
            break;
        default:
            current_cursor->col++;

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

        focused_window->display_text(document_text.get_text());
        focused_window->move_cursor(current_cursor->row, current_cursor->col);
    }
}
