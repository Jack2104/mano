#pragma once

#include "ncpp/Window.h"

namespace ncpp
{
    class Layout
    {
    public:
        Layout();
        Layout(int height, int width);

        /* Returns Layout& to allow chaining, i.e. layout.add(win1).add(win2) */
        Layout &add(std::shared_ptr<Window> window, int layer_y, int layer_x);
        void refresh();

    protected:
        bool fullscreen = false;

        int layout_width;
        int layout_height;

        int total_width;
        int total_height;

        // kinda cursed, should refactor this at some point.
        // Outer map is keyed on y position (rows) of windows, inner map is keyed on x position
        // (column) of windows
        std::map<int, std::map<int, std::shared_ptr<Window>>> y_layers;
    };
} /* namespace ncpp */