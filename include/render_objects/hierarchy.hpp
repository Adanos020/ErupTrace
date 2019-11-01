// BIH implementation based on the paper
// Instant Ray Tracing: The Bounding Interval Hierarchy
// by Carsten Wächter and Alexander Keller
// from the University of Ulm
// http://ainc.de/Research/BIH.pdf

#pragma once

#include <render_objects/shapes.hpp>
#include <util/numeric_types.hpp>

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

inline static std::pair<axis_aligned_box, extent_3d<float>> calculate_bounds_and_resolution(
    const std::vector<shape>& in_shapes, const float in_resolution_scale)
{
    extent_3d<float> resolution = { 0.f, 0.f, 0.f };
    if (in_shapes.empty())
    {
        return { axis_aligned_box::zero(), resolution };
    }

    axis_aligned_box scene_bounds = in_shapes.front().bounding_box;
    for (const shape& s : in_shapes)
    {
        if (s.bounding_box.min.x < scene_bounds.min.x) { scene_bounds.min.x = s.bounding_box.min.x; }
        if (s.bounding_box.min.y < scene_bounds.min.y) { scene_bounds.min.y = s.bounding_box.min.y; }
        if (s.bounding_box.min.z < scene_bounds.min.z) { scene_bounds.min.z = s.bounding_box.min.z; }
        if (s.bounding_box.max.x > scene_bounds.max.x) { scene_bounds.max.x = s.bounding_box.max.x; }
        if (s.bounding_box.max.y > scene_bounds.max.y) { scene_bounds.max.y = s.bounding_box.max.y; }
        if (s.bounding_box.max.z > scene_bounds.max.z) { scene_bounds.max.z = s.bounding_box.max.z; }
        resolution.width  += s.bounding_box.max.x - s.bounding_box.min.x;
        resolution.height += s.bounding_box.max.y - s.bounding_box.min.y;
        resolution.depth  += s.bounding_box.max.z - s.bounding_box.min.z;
    }
    resolution.width  /= float(in_shapes.size()) / in_resolution_scale;
    resolution.height /= float(in_shapes.size()) / in_resolution_scale;
    resolution.depth  /= float(in_shapes.size()) / in_resolution_scale;
    return { scene_bounds, resolution };
}

inline static std::vector<bih_node> make_hierarchy(std::vector<shape>& in_shapes, const float in_resolution_scale)
{
    const auto [scene_bounds, resolution] = calculate_bounds_and_resolution(in_shapes, in_resolution_scale);

    std::vector<bih_node> hierarchy;
    return hierarchy;
}