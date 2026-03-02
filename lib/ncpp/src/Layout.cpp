#include "ncpp/Layout.h"

namespace ncpp
{
    Layout::Layout() : layout_height(ncpp::rows()), layout_width(ncpp::cols()), fullscreen(true) {};

    Layout::Layout(int height, int width) : layout_height(height), layout_width(width), fullscreen(false) {};

    Layout &Layout::add(std::shared_ptr<Window> window, int layer_y, int layer_x)
    {
        if (!y_layers.contains(layer_y))
        {
            std::map<int, std::shared_ptr<Window>> window_map;
            y_layers.insert({layer_y, window_map});
        }

        y_layers[layer_y].insert({layer_x, window});

        return *this;
    }

    void Layout::refresh()
    {
        if (fullscreen)
        {
            layout_height = ncpp::rows();
            layout_width = ncpp::cols();
        }

        /* Calculate the total height taken up by fixed-height (i.e. non-expanding) windows. */
        int total_height = 0;
        int expanding_y_layers = 0;

        for (auto y_layer : y_layers)
        {
            /* Find the window with the largest height in this layer. */
            int max_layer_height = 0;
            bool layer_has_expanding_item = false;

            for (auto x_layer : y_layer.second)
            {
                if (x_layer.second->expands_vertically())
                    layer_has_expanding_item = true;
                else
                    max_layer_height = std::max(max_layer_height, x_layer.second->get_height());
            }

            total_height += max_layer_height;

            if (max_layer_height == 0 && layer_has_expanding_item)
                expanding_y_layers++;
        }

        /* Divide the remaining height among all expanding y layers. */
        int remaining_height = layout_height - total_height;
        int height_per_expanding_layer = expanding_y_layers > 0 ? remaining_height / expanding_y_layers : 0;
        int curr_row = 0;

        for (auto y_layer : y_layers)
        {
            int max_layer_height = 0;

            /* Calculate the width of any expanding x layers. */
            int expanding_x_layers = 0;
            int total_width = 0;

            for (auto x_layer : y_layer.second)
            {
                if (x_layer.second->expands_horizontally())
                    expanding_x_layers++;
                else
                    total_width += x_layer.second->get_width();

                if (!x_layer.second->expands_vertically())
                    max_layer_height = std::max(max_layer_height, x_layer.second->get_height());
            }

            /* Divide the remaining width among all expanding x layers. */
            int remaining_width = layout_width - total_width;
            int width_per_expanding_layer = expanding_x_layers > 0 ? remaining_width / expanding_x_layers : 0;
            int curr_col = 0;

            for (auto x_layer : y_layer.second)
            {
                int new_height = x_layer.second->get_height();

                if (x_layer.second->expands_vertically())
                    new_height = max_layer_height == 0 ? height_per_expanding_layer : max_layer_height;

                int new_width = x_layer.second->expands_horizontally() ? width_per_expanding_layer : x_layer.second->get_width();

                x_layer.second->reposition(curr_row, curr_col);
                x_layer.second->resize(new_height, new_width);

                curr_col += new_width;
            }

            curr_row += (max_layer_height == 0 ? height_per_expanding_layer : max_layer_height);
        }
    }
} /* namespace ncpp */