#include "TextBuffer.h"

TextBuffer::TextBuffer()
{
    gap_len = GAP_SIZE;
    buffer_len = gap_len;
    buffer.resize(buffer_len);

    cursor_pos = 0;
    current_line = 0;

    line_data.push_back(LineInfo{0, 0});
}

void TextBuffer::set_cursor_pos(Cursor cursor)
{
    int line_index = std::min(cursor.col, static_cast<int>(line_data.size())) - 1;
    LineInfo line = line_data[line_index];
    int offset = std::min(cursor.col, line.length) - 1;

    cursor_pos = line.start_index + std::max(0, offset);
}

void TextBuffer::insert(char c)
{
    move_gap();

    buffer[cursor_pos] = c;
    cursor_pos++;
    gap_pos++;
    gap_len--;

    /* Update all existing line info. */
    update_line_length(current_line, 1);

    if (c == '\n')
    {
        LineInfo new_line;
        new_line.start_index = line_data[current_line].start_index + line_data[current_line].length;
        new_line.length = 0;

        current_line++;
        line_data.insert(line_data.begin() + current_line, new_line);
    }
}

void TextBuffer::pop()
{
    move_gap();

    if (empty())
        return;

    cursor_pos--;
    gap_pos--;
    gap_len++;

    update_line_length(current_line, -1);

    /* If crossing a line boundary, combine the line info into one. */
    if (buffer[cursor_pos] == '\n')
    {
        line_data[current_line - 1].length += line_data[current_line].length;
        current_line--;
    }

    /* Remove line if it has no length. */
    if (line_data[current_line].length == 0)
    {
        line_data.erase(line_data.begin() + current_line);
    }
}

std::string TextBuffer::get_text()
{
    auto gap_start = buffer.begin() + gap_pos;
    auto gap_end = buffer.begin() + gap_pos + gap_len;

    /* Check if there's no text after the gap. */
    if (gap_end == buffer.end())
        return std::string(buffer.begin(), gap_start);
    // return std::to_string(buffer.size()) + ": " + std::to_string(gap_pos) + ", " + std::to_string(gap_len);

    return std::string(buffer.begin(), gap_start) + std::string(gap_end, buffer.end());
}

bool TextBuffer::empty()
{
    return cursor_pos == 0;
}

void TextBuffer::update_line_length(int line_num, int delta)
{
    line_data[line_num].length += delta;
    std::for_each(line_data.begin() + line_num + 1, line_data.end(), [delta](LineInfo &l)
                  { l.start_index += delta; });
}

void TextBuffer::move_gap()
{
    /* Move cursor on write. */
    if (cursor_pos != gap_pos)
    {
        /* Remove current gap. */
        int gap_end = gap_pos + gap_len - 1;
        std::shift_left(buffer.begin() + gap_end, buffer.end(), gap_len);

        /* Add new gap. */
        gap_pos = cursor_pos;
        std::shift_right(buffer.begin() + gap_pos, buffer.end(), gap_len);
    }

    /* Resize cursor on write. */
    if (gap_len == 0)
    {
        gap_len = GAP_SIZE;
        buffer_len += gap_len;

        buffer.resize(buffer_len);
        std::shift_right(buffer.begin() + gap_pos, buffer.end(), gap_len);
    }
}

// 0  1  2  3  4  5  6
// h  [  ]  e  l  l  0

// 0  1  2  3  4
// h  e  l  l  0

// 0 1 2 3 4
// a