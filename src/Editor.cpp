#include "Editor.h"

#include <limits.h>
#include <fstream>

Editor::Editor()
{
    nc::init();

    document_text = std::make_shared<TextBuffer>();
    cmd_bar_text = std::make_shared<TextBuffer>();

    document_cursor = std::make_shared<Cursor>(0, 0);
    cmd_bar_cursor = std::make_shared<Cursor>(0, 0);

    title_bar = std::make_shared<nc::Window>(1, nc::cols(), 0, 0);
    gutter = std::make_shared<nc::Window>(nc::rows() - 2, 5, 1, 0, "~");
    document_win = std::make_shared<nc::Window>(nc::rows() - 2, nc::cols() - 5, 1, 5);
    cmd_bar_win = std::make_shared<nc::Window>(1, nc::cols(), nc::rows() - 1, 0);

    title_bar->display_text("title bar");
    document_win->display_text(document_text->get_text());
    cmd_bar_win->display_text("command bar");

    gutter->set_horizontal_expansion(false);
    gutter->set_vertical_expansion(true);
    set_line_numbers(1, 1);

    document_win->set_vertical_expansion(true);
    document_win->move_cursor(*document_cursor);

    layout.add(title_bar, 0, 0).add(gutter, 1, 0).add(document_win, 1, 1).add(cmd_bar_win, 2, 0);
    layout.refresh();

    document_ctx = Context(document_text, document_cursor, document_win);
    cmd_bar_ctx = Context(cmd_bar_text, cmd_bar_cursor, cmd_bar_win);
    current_ctx = document_ctx;

    contexts.insert({Mode::EDITING, document_ctx});
    contexts.insert({Mode::GOTO, cmd_bar_ctx});
}

Editor::~Editor()
{
    nc::cleanup();
}

void Editor::set_cursor_pos(const Cursor &new_cursor)
{
    int new_row = new_cursor.row;
    int new_col = new_cursor.col;

    /* Enforce contstraints to ensure the cursor doesn't go beyond the line/document length. */
    int max_row = std::max(0, current_ctx.text->get_line_count() - 1);

    if (new_row > max_row)
        new_row = max_row;
    else if (new_row < 0)
        new_row = 0;

    int max_col = std::max(0, current_ctx.text->get_line_length(new_row) - get_line_end_offset(new_row));

    if (new_col > max_col)
        new_col = max_col;
    else if (new_col < 0)
        new_col = 0;

    current_ctx.cursor->row = new_row;
    current_ctx.cursor->col = new_col;

    current_ctx.window->move_cursor(*current_ctx.cursor);
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

void Editor::update_cursor(int key)
{
    int current_row = current_ctx.cursor->row;
    int current_col = current_ctx.cursor->col;

    int new_row = current_row;
    int new_col = current_col;

    /* Calculate new row and column values, without regard for any contraints. */
    switch (key)
    {
    case KEY_DOWN:
        new_row = current_row + 1;
        break;
    case KEY_UP:
        new_row = current_row - 1;
        break;
    case KEY_LEFT:
        if (current_col <= 0 && current_row > 0)
        {
            new_row = current_row - 1;
            new_col = std::numeric_limits<int>::max(); // Will be constrained below
        }
        else
        {
            new_col = current_col - 1;
        }

        break;
    case KEY_RIGHT:
        if (current_col >= (current_ctx.text->get_line_length(current_row) - get_line_end_offset(current_row)) &&
            !current_ctx.text->is_final_line(current_row))
        {
            new_row = current_row + 1;
            new_col = 0;
        }
        else
        {
            new_col = current_col + 1;
        }

        break;
    };

    set_cursor_pos(Cursor{new_row, new_col});
}

int Editor::get_line_end_offset(int line_num)
{
    /* Avoids counting the newline as part of line length, aside from the final
    line (as the final line does not have a newline). */
    return current_ctx.text->is_final_line(line_num) ? 0 : 1;
}

void Editor::change_state(Mode new_state)
{
    current_state = new_state;
    current_ctx = contexts[new_state];
    current_ctx.window->move_cursor(*current_ctx.cursor);
}

void Editor::start_state_machine()
{
    while (true)
    {
        /* Conceptually a character, but int is used (ncurses does this, so we do too). */
        int input = current_ctx.window->get_input();

        switch (input)
        {
        case ERR:
            continue;
        case KEY_RESIZE:
            layout.refresh();
            break;
        case KEY_BACKSPACE:
        case 127:
        case '\b':
            /* Only update line numbers if the line count has changed. */
            // if (current_cursor->col == 0)
            //     set_line_numbers(1, current_text->get_line_count());

            update_cursor(KEY_LEFT);
            current_ctx.text->pop();
            set_line_numbers(1, current_ctx.text->get_line_count());

            current_ctx.window->display_text(current_ctx.text->get_text());
            current_ctx.text->set_cursor_pos(current_ctx.cursor->row, current_ctx.cursor->col);
            break;
        case KEY_DOWN:
        case KEY_UP:
        case KEY_LEFT:
        case KEY_RIGHT:
            update_cursor(input);
            current_ctx.text->set_cursor_pos(current_ctx.cursor->row, current_ctx.cursor->col);
            break;
        case nc::CTRL_C:
        case nc::CTRL_X:
        case nc::CTRL_Q:
            return;
            break;
        case nc::CTRL_G:
            if (current_state == Mode::GOTO)
            {
                current_ctx.text->clear();
                change_state(Mode::EDITING);
            }
            else if (current_state == Mode::EDITING)
            {
                change_state(Mode::GOTO);
                set_cursor_pos(Cursor{0, 0});
            }
            break;
        case '\r':
            break;
        case '\n':
            if (current_state == Mode::GOTO)
            {
                auto [row_opt, col_opt] = parse_goto_command(current_ctx.text->get_text());

                Cursor new_cursor;
                new_cursor.row = row_opt ? *row_opt : current_ctx.cursor->row;
                new_cursor.col = col_opt ? *col_opt : current_ctx.cursor->col;

                current_ctx.text->clear();
                change_state(Mode::EDITING);
                set_cursor_pos(new_cursor);

                break;
            }

            current_ctx.text->insert(static_cast<char>(input));

            if (current_state == Mode::EDITING)
                set_line_numbers(1, current_ctx.text->get_line_count());

            update_cursor(KEY_DOWN);
            current_ctx.cursor->col = 0;

            current_ctx.window->display_text(current_ctx.text->get_text());
            break;
        default:
            current_ctx.text->insert(static_cast<char>(input));
            update_cursor(KEY_RIGHT);
            current_ctx.window->display_text(current_ctx.text->get_text());
            break;
        };

        if (current_state == Mode::EDITING)
            cmd_bar_win->display_text(std::to_string(current_ctx.cursor->row + 1) + ":" + std::to_string(current_ctx.cursor->col + 1));

        current_ctx.window->display_text(current_ctx.text->get_text());
        current_ctx.window->move_cursor(*current_ctx.cursor);
    }
}

std::pair<std::optional<int>, std::optional<int>> Editor::parse_goto_command(std::string command)
{
    std::string::size_type delim_pos = command.find(':');

    std::optional<int> row;

    try
    {
        row = std::stoi(command.substr(0, delim_pos)) - 1;
    }
    catch (std::invalid_argument)
    {
        row = std::nullopt;
    }

    if (delim_pos == std::string::npos)
        return std::make_pair(row, std::nullopt);

    std::optional<int> col;

    try
    {
        col = std::stoi(command.substr(delim_pos, command.length())) - 1;
    }
    catch (std::invalid_argument)
    {
        col = std::nullopt;
    }

    return std::make_pair(row, col);
}

// To handle cursor/view window: simply keep track of how many lines above/below cursor position to
// show. Put constraints on these numbers (max is window height, min is zero). Then subtract/add to
// these values when cursor is moved.
// Then, for example, if the cursor is at the bottom it will stay visually at the bottom since the
// lines above/below will be constrained and so won't change. If the cursor is then moved up, the
// lines above will be subtraced from and lines below added to, which won't be constrained and so
// the cursor will visually move upwards.
//
// Can render certain lines by using start indexes in TextMetadata

// TODO: add separate enums for Mode (editing, command) and CommandMode (goto, saving, etc.)