#include "TextBuffer.h"

#include <fstream>

TextBuffer::TextBuffer()
{
    gap_len = GAP_SIZE;
    buffer.resize(gap_len);

    buffer_space_cursor_pos = 0;
    gap_pos = 0;
    current_line = 0;

    line_data.push_back(LineInfo{0, 0});

    debug();
}

void TextBuffer::set_cursor_pos(int row, int col)
{
    int max_line = static_cast<int>(line_data.size()) - 1;
    current_line = std::min(row, max_line);

    LineInfo line = line_data[current_line];
    int offset = std::min(col, line.length);
    buffer_space_cursor_pos = to_buffer_space(line.start_index + std::max(0, offset));

    debug();
}

void TextBuffer::insert(char c)
{
    move_gap();

    buffer[buffer_space_cursor_pos] = c;
    buffer_space_cursor_pos++;
    gap_pos++;
    gap_len--;

    if (c == '\n')
    {
        LineInfo new_line;
        new_line.start_index = to_text_space(buffer_space_cursor_pos);

        int line_end = line_data[current_line].start_index + line_data[current_line].length + 1;
        new_line.length = line_end - new_line.start_index;
        line_data[current_line].length -= new_line.length;

        current_line++;
        line_data.insert(line_data.begin() + current_line, new_line);

        std::for_each(line_data.begin() + current_line + 1, line_data.end(), [](LineInfo &l)
                      { l.start_index++; });
    }
    else
    {
        /* Update all existing line info. */
        update_line_length(current_line, 1);
    }

    debug();
}

void TextBuffer::pop()
{
    move_gap();

    if (buffer_space_cursor_pos == 0 || empty())
        return;

    buffer_space_cursor_pos--;
    gap_pos--;
    gap_len++;

    /* Update all existing line info. */

    /* If crossing a line boundary, combine the line info into one. */
    if (buffer[buffer_space_cursor_pos] == '\n')
    {
        line_data[current_line - 1].length += line_data[current_line].length;
        line_data.erase(line_data.begin() + current_line);
        current_line--;

        std::for_each(line_data.begin() + current_line + 1, line_data.end(), [](LineInfo &l)
                      { l.start_index--; });
    }
    else
    {
        // TODO: we dont actually need to update line start indexes on deletion as
        // the deletions just create a larger gap. we do however need to update them
        // when the gap grows (i.e. only on insertions) since the start indexes will
        // have moved forwards
        update_line_length(current_line, -1);
    }

    debug();
}

std::string TextBuffer::get_text()
{
    if (empty())
        return "";

    auto gap_start = buffer.begin() + gap_pos;
    auto gap_end = buffer.begin() + gap_pos + gap_len;

    /* Check if there's no text after the gap. */
    if (gap_end == buffer.end())
        return std::string(buffer.begin(), gap_start);

    return std::string(buffer.begin(), gap_start) + std::string(gap_end, buffer.end());
}

std::string TextBuffer::get_debug_text()
{
    auto gap_end = buffer.begin() + gap_pos + gap_len;
    bool gap_is_end = gap_end == buffer.end();

    return std::to_string(buffer.size()) + ": cursor = " + std::to_string(buffer_space_cursor_pos) + ", gap start = " + std::to_string(gap_pos) + ", gap len = " + std::to_string(gap_len) + ", gap end = " + std::to_string(gap_pos + gap_len) + ", gap end == buffer.end() = " + std::to_string(gap_is_end) + ", buffer size = " + std::to_string(buffer.size());
}

bool TextBuffer::empty()
{
    return buffer.size() == gap_len;
}

void TextBuffer::update_line_length(int line_num, int delta)
{
    line_data[line_num].length += delta;

    std::for_each(line_data.begin() + line_num + 1, line_data.end(), [delta](LineInfo &l)
                  { l.start_index += delta; });

    debug();
}

void TextBuffer::move_gap()
{
    /* Move cursor on write. */
    if (buffer_space_cursor_pos != gap_pos)
    {
        /* Remove current gap. */
        auto gap_start = buffer.begin() + gap_pos;
        std::shift_left(gap_start, buffer.end(), gap_len);

        /* If the cursor is beyond the gap, then its position has the gap
        factored into it. Therefore if the gap is removed, we need to adjust
        the cursor's position to account for the (now removed) gap. */
        if (buffer_space_cursor_pos > gap_pos)
            buffer_space_cursor_pos -= gap_len;

        /* Add new gap at new cursor position. */
        gap_pos = buffer_space_cursor_pos;
        std::shift_right(buffer.begin() + gap_pos, buffer.end(), gap_len);
    }

    /* Resize cursor on write. */
    if (gap_len == 0)
    {
        gap_len = GAP_SIZE;
        buffer.resize(buffer.size() + gap_len);
        std::shift_right(buffer.begin() + gap_pos, buffer.end(), gap_len);
    }

    debug();
}

bool TextBuffer::gap_at_end()
{
    auto gap_end = buffer.begin() + gap_pos + gap_len;
    return gap_end < buffer.end();
}

int TextBuffer::to_buffer_space(int text_space_pos)
{
    if (text_space_pos <= gap_pos)
        return text_space_pos;

    return text_space_pos + gap_len;
}

int TextBuffer::to_text_space(int buffer_space_pos)
{
    if (buffer_space_pos <= gap_pos)
        return buffer_space_pos;
    else if (buffer_space_pos >= gap_pos && buffer_space_pos < gap_pos + gap_len - 1)
        return gap_pos;

    return buffer_space_pos - gap_len;
}

void TextBuffer::debug()
{
    std::ofstream debug_file("debug.txt", std::ofstream::out | std::ofstream::trunc);

    debug_file << "= Cursor = " << std::endl;
    debug_file << "cursor pos = " << buffer_space_cursor_pos << ", cursor line = " << current_line << std::endl;
    debug_file << "at: " << buffer[buffer_space_cursor_pos] << std::endl;

    debug_file << "\n= Gap = " << std::endl;
    debug_file << "gap len = " << gap_len << ", gap pos = " << gap_pos << std::endl;

    debug_file << "\n= Buffer =" << std::endl;

    for (int i = 0; i < buffer.size(); i++)
    {
        if (i >= gap_pos && i < gap_pos + gap_len)
        {
            debug_file << i << ": _ (gap)";
        }
        else if (buffer[i] == '\n')
        {
            debug_file << i << ": \\n";
        }
        else
        {
            debug_file << i << ": " << buffer[i];
        }

        if (i == buffer_space_cursor_pos)
            debug_file << " (cursor)";

        debug_file << std::endl;
    }

    debug_file << "\n= Line Info =" << std::endl;

    for (auto &line : line_data)
    {
        debug_file << "start index = " << line.start_index << ", length = " << line.length << std::endl;
    }
}

// 0 1 2 3 4 (end)
// b       a

// 0 1 2 3 4 5 6 7 8 9 10 (end)
// a b c d e f g h i j k

// first = 9, last = 11, n = 1
// first - last = 11 - 9 = 3
// first - last - n = 11 - 9 - 1 = 2