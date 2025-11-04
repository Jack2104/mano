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

    void set_cursor_pos(Cursor cursor);
    void insert(char c);
    void remove();
    std::string get_text();

private:
    std::vector<char> buffer;
    int buffer_len;

    const int GAP_SIZE = 25;
    int gap_len;
    int gap_pos;

    int cursor_pos;
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
};