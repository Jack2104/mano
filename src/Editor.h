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

    std::shared_ptr<TextBuffer> document_text;
    std::shared_ptr<TextBuffer> command_text;
    std::shared_ptr<TextBuffer> current_text;

    std::shared_ptr<Cursor> editor_cursor;
    std::shared_ptr<Cursor> command_cursor;
    std::shared_ptr<Cursor> current_cursor;

    std::shared_ptr<nc::Window> title_bar;
    std::shared_ptr<nc::Window> gutter;
    std::shared_ptr<nc::Window> editor;
    std::shared_ptr<nc::Window> command_bar;
    std::shared_ptr<nc::Window> focused_window;

    nc::Layout layout = nc::Layout();

    void set_line_numbers(int start_num, int end_num);
    void update_cursor(int key);
    int get_line_end_offset(int line_num);
};
