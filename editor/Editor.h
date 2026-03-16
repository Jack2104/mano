#pragma once

#include <ncpp/ncpp.h>
#include <ncpp/Window.h>
#include <ncpp/Layout.h>
#include <text_buffer/TextBuffer.h>
#include <io_backend/IOBackend.h>

#include <optional>
#include <unordered_map>
#include <memory>

enum class Mode
{
    EDITING,
    GOTO,
    SAVING
};

class Context
{
public:
    Context() : text(nullptr), cursor(nullptr), window(nullptr) {};
    Context(std::shared_ptr<TextBuffer> text,
            std::shared_ptr<Cursor> cursor,
            std::shared_ptr<ncpp::Window> window) : text(text), cursor(cursor), window(window) {};

    std::shared_ptr<TextBuffer> text;
    std::shared_ptr<Cursor> cursor;
    std::shared_ptr<ncpp::Window> window;
};

class Editor
{
public:
    Editor();
    Editor(IOBackend *io_backend) : backend(io_backend) {};
    ~Editor();

    void start_state_machine();

private:
    IOBackend *backend;
    Mode current_state = Mode::EDITING;

    std::shared_ptr<TextBuffer> document_text;
    std::shared_ptr<TextBuffer> cmd_bar_text;

    std::shared_ptr<Cursor> document_cursor;
    std::shared_ptr<Cursor> cmd_bar_cursor;

    int prev_column = 0;

    std::shared_ptr<ncpp::Window> title_bar;
    std::shared_ptr<ncpp::Window> gutter;
    std::shared_ptr<ncpp::Window> document_win;
    std::shared_ptr<ncpp::Window> cmd_bar_win;

    ncpp::Layout layout = ncpp::Layout();

    Context document_ctx;
    Context cmd_bar_ctx;
    Context current_ctx;

    std::unordered_map<Mode, Context> contexts;

    char command_delim = ':';

    bool saved = true;
    std::string file_path = "";

    void set_cursor_pos(const Cursor &new_cursor);
    void change_state(Mode new_state);

    void set_line_numbers(int start_num, int end_num);
    void update_cursor(int key);
    int get_line_end_offset(int line_num);

    std::pair<std::optional<int>, std::optional<int>> parse_goto_command(std::string command);
};
