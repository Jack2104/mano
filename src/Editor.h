#pragma once

#include "ncurses_utilities.h"
#include "TextBuffer.h"

enum class Mode
{
    EDITING,
    GOTO,
    CONFIRMATION
};

class Editor
{
public:
    Editor();
    ~Editor();

    void start_state_machine();

private:
    Mode current_mode = Mode::EDITING;
    TextBuffer document_text = TextBuffer();

    std::shared_ptr<Cursor> editor_cursor;
    std::shared_ptr<Cursor> command_cursor;

    std::shared_ptr<Cursor> current_cursor;

    std::shared_ptr<nc::Window> title_bar;
    std::shared_ptr<nc::Window> gutter;
    std::shared_ptr<nc::Window> editor;
    std::shared_ptr<nc::Window> command_bar;

    std::shared_ptr<nc::Window> focused_window;

    nc::Layout layout = nc::Layout();

    int line_count = 1;

    void set_line_numbers(int start_num, int end_num);
};