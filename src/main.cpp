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
