// BIH implementation based on papers:
// 
// Instant Ray Tracing: The Bounding Interval Hierarchy
// Carsten Wächter and Alexander Keller
// University of Ulm
// http://ainc.de/Research/BIH.pdf
// 
// GPU Volume Raycasting using Bounding Interval Hierarchies
// Martin Kinkelin (0326997)
// Masterpraktikum aus Computergraphik und Digitaler Bildverarbeitung
// https://pdfs.semanticscholar.org/f110/495938b5f150fe7cf605e14dd01a30bd6290.pdf

#include <render_objects/hierarchy.hpp>

static std::pair<axis_aligned_box, extent_3d<float>> calculate_bounds_and_resolution(
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
        scene_bounds.min.x = std::min(s.bounding_box.min.x, scene_bounds.min.x);
        scene_bounds.min.y = std::min(s.bounding_box.min.y, scene_bounds.min.y);
        scene_bounds.min.z = std::min(s.bounding_box.min.z, scene_bounds.min.z);
        scene_bounds.max.x = std::max(s.bounding_box.max.x, scene_bounds.max.x);
        scene_bounds.max.y = std::max(s.bounding_box.max.y, scene_bounds.max.y);
        scene_bounds.max.z = std::max(s.bounding_box.max.z, scene_bounds.max.z);
        resolution.width += s.bounding_box.max.x - s.bounding_box.min.x;
        resolution.height += s.bounding_box.max.y - s.bounding_box.min.y;
        resolution.depth += s.bounding_box.max.z - s.bounding_box.min.z;
    }

    const float scale = in_resolution_scale / float(in_shapes.size());
    resolution.width *= scale;
    resolution.height *= scale;
    resolution.depth *= scale;
    return { scene_bounds, resolution };
}

bounding_interval_hierarchy make_hierarchy(std::vector<shape>& in_shapes, const float in_resolution_scale)
{
    const auto [scene_bounds, resolution] = calculate_bounds_and_resolution(in_shapes, in_resolution_scale);

    bounding_interval_hierarchy hierarchy;
    return hierarchy;
}