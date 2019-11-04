#pragma once

#include <render_objects/shapes.hpp>

#include <vector>

struct bih_node
{
    enum class node_type : uint32_t { x, y, z, leaf } type;
    union
    {
        uint32_t index;
        float clip[2];
    };
};

using bounding_interval_hierarchy = std::vector<bih_node>;

bounding_interval_hierarchy make_hierarchy(std::vector<shape>& in_shapes, float in_resolution_scale);