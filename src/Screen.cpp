#include "Screen.h"

Screen::Screen(int max_height, int max_width) : height(max_height), width(max_width)
{
    title_bar = newwin(1, width, 0, 0);
    editor = newwin(height - 2, width, 1, 0);
    command_bar = newwin(1, width, height - 1, 0);
    box(title_bar, '|', '-');
    box(editor, '|', '-');
    box(command_bar, '|', '-');

    keypad(editor, true);
    keypad(command_bar, true);
}

Screen::~Screen()
{
    delwin(title_bar);
    delwin(editor);
    delwin(command_bar);
}

void Screen::render()
{
    // resize windows
    refresh();
}

void Screen::set_display_text(std::string text)
{
    // resize window
    // display text
    werase(editor);
    wprintw(editor, text.c_str());
    wrefresh(editor);
}

void Screen::set_title_bar_text(std::string text)
{
    // resize window
    // display text
    werase(title_bar);
    wprintw(title_bar, text.c_str());
    wrefresh(title_bar);
}

void Screen::set_command_bar_text(std::string text)
{
    // resize window
    // display text
    werase(command_bar);
    wprintw(command_bar, text.c_str());
    wrefresh(command_bar);
}

void Screen::set_cursor_pos(int row, int col)
{
}

int Screen::get_editor_input()
{
    return wgetch(editor);
}

int Screen::get_command_bar_input()
{
    return wgetch(command_bar);
}

DisplayArea Screen::get_display_area()
{
}
