#pragma once

#include <vector>
#include <iostream>

struct LineMetadata
{
    int start_index;
    int length;
    bool final_line;
};

class TextMetadata
{
public:
    TextMetadata();

    /* Used for adding lines. Splits the line at line_num into two at index, where the character
    at index becomes the first character of the next line. If index is the end of a line,
    effectively just "adds" a new empty line below line_num. */
    void split_line(int line_num, int index);

    /* Used for removing lines. Merges the line at line_num into the line above it. If the line at
    line_num is empty, it effectively just "deletes" the line at line_num. */
    void merge_line(int line_num);

    /* Updates the length of the line at line_num by delta. */
    void update_line_length(int line_num, int delta);

    /* Updates the start indexes of all of the lines from start_line_num by delta. Used to keep
    start indexes consistent when adding or removing lines. */
    void update_indexes(int start_line_num, int delta);

    void clear();

    /* Getters. */
    int line_start_index(int line_num);
    int line_length(int line_num);
    int line_count();
    bool line_is_final(int line_num);

    friend std::ostream &operator<<(std::ostream &os, TextMetadata tm);

private:
    // TODO: look into using a fenwick tree for this
    std::vector<LineMetadata> line_data;
};
