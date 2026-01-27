#pragma once

#include "ncurses_utilities.h"
#include "TextBuffer.h"

#include <optional>
#include <unordered_map>

enum class Mode
{
    EDITING,
    GOTO,
    CONFIRMATION
};

class Context
{
public:
    Context() : text(nullptr), cursor(nullptr), window(nullptr) {};
    Context(std::shared_ptr<TextBuffer> text,
            std::shared_ptr<Cursor> cursor,
            std::shared_ptr<nc::Window> window) : text(text), cursor(cursor), window(window) {};

    std::shared_ptr<TextBuffer> text;
    std::shared_ptr<Cursor> cursor;
    std::shared_ptr<nc::Window> window;
};

class Editor
{
public:
    Editor();
    ~Editor();

    void start_state_machine();

private:
    Mode current_state = Mode::EDITING;

    std::shared_ptr<TextBuffer> document_text;
    std::shared_ptr<TextBuffer> cmd_bar_text;

    std::shared_ptr<Cursor> document_cursor;
    std::shared_ptr<Cursor> cmd_bar_cursor;

    std::shared_ptr<nc::Window> title_bar;
    std::shared_ptr<nc::Window> gutter;
    std::shared_ptr<nc::Window> document_win;
    std::shared_ptr<nc::Window> cmd_bar_win;

    nc::Layout layout = nc::Layout();

    Context document_ctx;
    Context cmd_bar_ctx;
    Context current_ctx;

    std::unordered_map<Mode, Context> contexts;

    char command_delim = ':';

    void set_cursor_pos(const Cursor &new_cursor);
    void change_state(Mode new_state);

    void set_line_numbers(int start_num, int end_num);
    void update_cursor(int key);
    int get_line_end_offset(int line_num);

    std::pair<std::optional<int>, std::optional<int>> parse_goto_command(std::string command);
};
