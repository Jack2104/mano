#include "TextBuffer.h"

#include <fstream>

TextBuffer::TextBuffer()
{
    clear();

    debug();
}

void TextBuffer::set_cursor_pos(int row, int col)
{
    int max_line = metadata.line_count() - 1; // Lines are zero-indexed, so subtract 1
    current_line = std::min(row, max_line);

    /* Lines without a newline (i.e. the final line) allow the cursor to move one index beyond the
    final text index, to allow for inserting at the end of the line. */
    int selectable_indices = metadata.line_length(current_line);

    /* Lines with a newline make the cursor stop at the newline (since it's invalid to insert
    characters after a newline on a single line), and so subtract one from the line length to
    account for the newline. */
    if (!metadata.line_is_final(current_line))
        selectable_indices--;

    int offset = std::min(col, selectable_indices);

    int text_space_cursor_pos = metadata.line_start_index(current_line) + std::max(0, offset);
    buffer_space_cursor_pos = to_buffer_space(text_space_cursor_pos);

    debug();
}

void TextBuffer::insert(char c)
{
    move_gap();

    buffer[buffer_space_cursor_pos] = c;
    buffer_space_cursor_pos++;
    gap_pos++;
    gap_len--;

    metadata.update_line_length(current_line, 1);

    /* Split the line into two (or create a new one) on a newline. */
    if (c == '\n')
    {
        int text_space_cursor_pos = to_text_space(buffer_space_cursor_pos);
        int relative_index = text_space_cursor_pos - metadata.line_start_index(current_line);

        metadata.split_line(current_line, relative_index);
        current_line++;
    }

    debug();
}

void TextBuffer::pop()
{
    move_gap();

    if (buffer_space_cursor_pos == 0 || is_empty())
        return;

    buffer_space_cursor_pos--;
    gap_pos--;
    gap_len++;

    /* If crossing a line boundary, combine the lines into one. */
    if (buffer[buffer_space_cursor_pos] == '\n')
    {
        metadata.merge_line(current_line);
        current_line--;
    }

    metadata.update_line_length(current_line, -1);

    debug();
}

void TextBuffer::clear()
{
    metadata.clear();

    gap_len = GAP_SIZE;
    buffer.resize(gap_len);

    buffer_space_cursor_pos = 0;
    gap_pos = 0;
    current_line = 0;
}

std::string TextBuffer::get_text()
{
    if (is_empty())
        return "";

    auto gap_start = buffer.begin() + gap_pos;
    auto gap_end = buffer.begin() + gap_pos + gap_len;

    /* Check if there's no text after the gap. */
    if (gap_end == buffer.end())
        return std::string(buffer.begin(), gap_start);

    return std::string(buffer.begin(), gap_start) + std::string(gap_end, buffer.end());
}

bool TextBuffer::is_empty()
{
    return buffer.size() == gap_len;
}

int TextBuffer::get_line_count()
{
    return metadata.line_count();
}

int TextBuffer::get_line_length(int line_num)
{
    return metadata.line_length(line_num);
}

bool TextBuffer::is_final_line(int line_num)
{
    return metadata.line_is_final(line_num);
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
    debug_file << "buffer space cursor pos = " << buffer_space_cursor_pos << ", cursor line = " << current_line << std::endl;
    debug_file << "text space cursor pos = " << to_text_space(buffer_space_cursor_pos) << std::endl;
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

    debug_file << metadata;
}
