#pragma once

#include <string>
#include <vector>

#include "TextMetadata.h"

struct Cursor
{
    int row;
    int col;
};

class TextBuffer
{
public:
    TextBuffer();

    void set_cursor_pos(int row, int col);

    void insert(char c);
    void pop();
    void clear();

    std::string get_text();
    bool is_empty();
    int get_line_count();
    int get_line_length(int line_num);
    bool is_final_line(int line_num);

private:
    std::vector<char> buffer;
    int buffer_len;

    const int GAP_SIZE = 5;
    int gap_len;
    int gap_pos;

    int buffer_space_cursor_pos;
    int current_line;

    TextMetadata metadata = TextMetadata();

    void move_gap();
    bool gap_at_end();

    /* There are two "positions" the cursor has. The "buffer space" position is the index of the
    cursor inside the raw buffer, which includes the gap and any implementation-specific details.
    The "text space" is the conceptual position of the cursor based on just the text, so without
    the gap or any implementation details. These functions convert from one to the other. */
    int to_buffer_space(int text_space_pos);
    int to_text_space(int buffer_space_pos);

    void debug();
};

// TODO:
//   - refactor TextBuffer to have no sense of "cursor position". Make it only appear as a random-access
//     text buffer. Move TextMetadata out of TextBuffer and use it alongside TextBuffer in Editor class,
//     so that Editor can see line information (such as line count, etc.)
//   - Then make "TextEdit" class that combines a Window and TextBuffer, to replace the editor and
//     command bar. This will take an optional "default string" (used for the command bar). May have
//     to also take a lambda to have dynamic default strings.
//       - Would need some internal layout, and
//         have it and Window inherit from a "Widget" interface. This interface would have functions
//         to set width/height. For TextEdit, this would modify the internal Layout. External Layout
//         would then indirectly set this.
//     - maybe include line numbers in this?