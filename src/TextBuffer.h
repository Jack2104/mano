#pragma once

#include <string>
#include <vector>

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
    std::string get_text();
    std::string get_debug_text();
    bool empty();

private:
    std::vector<char> buffer;
    int buffer_len;

    const int GAP_SIZE = 5;
    int gap_len;
    int gap_pos;

    int buffer_space_cursor_pos;
    int current_line;

    // TODO: look into using a fenwick tree for this
    struct LineInfo
    {
        int start_index;
        int length;
    };

    std::vector<LineInfo> line_data;

    void update_line_length(int line_num, int delta);
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