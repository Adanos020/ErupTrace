#pragma once

#include <render_objects/shapes.hpp>

#include <vector>

enum class BIH_node_type : uint32_t { x, y, z, leaf };

struct BIH_node
{
    BIH_node_type type;
    union
    {
        uint32_t shape_index;
        struct
        {
            struct { float left, right; } clip;
            struct { uint32_t left, right; } children;
        };
    };
};

using bounding_interval_hierarchy = std::vector<BIH_node>;

bounding_interval_hierarchy make_hierarchy(std::vector<shape>& in_shapes);
std::pair<bool, bool> line_intersects_children_of(const struct line&, const BIH_node&, const min_max<float>& distance);