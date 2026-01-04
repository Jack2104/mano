#include <cstdlib>
#include <ncurses.h>

#include <iostream>
#include <string>

#include "ncurses_utilities.h"
#include "TextBuffer.h"
#include "Editor.h"

int main(int argc, char *argv[])
{
    Editor editor = Editor();
    editor.start_state_machine();

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