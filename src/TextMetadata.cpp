#include "TextMetadata.h"

TextMetadata::TextMetadata()
{
    line_data.push_back(LineMetadata{0, 0, true});
}

void TextMetadata::split_line(int line_num, int index)
{
    if (line_num >= line_data.size())
        return;

    /* Create new line. */
    LineMetadata new_line;
    new_line.start_index = line_data[line_num].start_index + index;
    new_line.length = line_data[line_num].length - index;
    new_line.final_line = line_data[line_num].final_line;

    line_data.insert(line_data.begin() + line_num + 1, new_line);

    /* Update existing line. */
    line_data[line_num].length -= new_line.length;
    line_data[line_num].final_line = false;
}

void TextMetadata::merge_line(int line_num)
{
    if (line_num >= line_data.size())
        return;

    line_data[line_num - 1].length += line_data[line_num].length;
    line_data[line_num - 1].final_line = line_data[line_num].final_line;
    line_data.erase(line_data.begin() + line_num);
}

void TextMetadata::update_line_length(int line_num, int delta)
{
    if (line_num >= line_data.size())
        return;

    line_data[line_num].length += delta;

    update_indexes(line_num + 1, delta);
}

void TextMetadata::update_indexes(int start_line_num, int delta)
{
    auto start_itr = line_data.begin() + start_line_num;

    if (start_itr >= line_data.end())
        return;

    std::for_each(start_itr, line_data.end(), [delta](LineMetadata &l)
                  { l.start_index += delta; });
}

void TextMetadata::clear()
{
    line_data.clear();
}

int TextMetadata::line_start_index(int line_num)
{
    if (line_num >= line_data.size())
        return -1;

    return line_data[line_num].start_index;
}

int TextMetadata::line_length(int line_num)
{
    if (line_num < 0 || line_num >= line_data.size())
        return 0;

    return line_data[line_num].length;
}

int TextMetadata::line_count()
{
    return static_cast<int>(line_data.size());
}

bool TextMetadata::line_is_final(int line_num)
{
    return line_data[line_num].final_line;
}

std::ostream &operator<<(std::ostream &os, TextMetadata tm)
{
    for (auto &line : tm.line_data)
    {
        os << "start index = " << line.start_index << ", length = " << line.length << ", final line = " << line.final_line << std::endl;
    }

    return os;
}
