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
#include <util/numeric_types.hpp>
#include <util/pairs.hpp>

#include <algorithm>

// Construction

static BIH_node_type choose_split_axis(const axis_aligned_box& in_box)
{
    const extent_3D<float> box_size = in_box.size();
    if (box_size.width > box_size.height && box_size.width > box_size.depth)
    {
        return BIH_node_type::x;
    }
    if (box_size.height > box_size.depth)
    {
        return BIH_node_type::y;
    }
    return BIH_node_type::z;
}

static axis_aligned_box calculate_bounds(const std::vector<shape>& in_shapes)
{
    axis_aligned_box scene_bounds = in_shapes.front().bounding_box;

    std::for_each(std::next(in_shapes.cbegin()), in_shapes.cend(), [&](const shape& s)
    {
        scene_bounds.min.x = std::min(s.bounding_box.min.x, scene_bounds.min.x);
        scene_bounds.min.y = std::min(s.bounding_box.min.y, scene_bounds.min.y);
        scene_bounds.min.z = std::min(s.bounding_box.min.z, scene_bounds.min.z);
        scene_bounds.max.x = std::max(s.bounding_box.max.x, scene_bounds.max.x);
        scene_bounds.max.y = std::max(s.bounding_box.max.y, scene_bounds.max.y);
        scene_bounds.max.z = std::max(s.bounding_box.max.z, scene_bounds.max.z);
    });

    return scene_bounds;
}

static BIH_node_type split(
    const iterator_pair<std::vector<shape>>& in_shapes,
    BIH_node& out_current_node,
    std::vector<shape>::iterator& out_middle,
    axis_aligned_box& out_left_box,
    axis_aligned_box& out_right_box
) {
    for (
        uint32_t it = 0, axis = uint32_t(choose_split_axis(out_left_box));
        it < 3;
        ++it, axis = (axis + 1) % 3
    ) {
        const auto is_to_the_left = [axis, in_split_plane = out_left_box.origin()[axis]]
            (const shape& a) { return a.bounding_box.origin()[axis] < in_split_plane; };
        out_middle = std::partition(in_shapes.begin, in_shapes.end, is_to_the_left);

        if (out_middle != in_shapes.begin && out_middle != in_shapes.end)
        {
            const auto compare_max = [=](const shape& a, const shape& b) { return a.bounding_box.max[axis] < b.bounding_box.max[axis]; };
            const auto compare_min = [=](const shape& a, const shape& b) { return a.bounding_box.min[axis] < b.bounding_box.min[axis]; };
            const std::vector<shape>::iterator max_left = std::max_element(in_shapes.begin, out_middle, compare_max);
            const std::vector<shape>::iterator min_right = std::min_element(out_middle, in_shapes.end, compare_min);

            out_left_box.max[axis] = max_left->bounding_box.min[axis];
            out_right_box.min[axis] = min_right->bounding_box.max[axis];
            out_current_node.clip.left = max_left->bounding_box.max[axis];
            out_current_node.clip.right = min_right->bounding_box.min[axis];

            return out_current_node.type = BIH_node_type{ axis };
        }
    }
    return BIH_node_type::leaf;
}

static void make_hierarchy(
    const iterator_pair<std::vector<shape>>& in_shapes,
    std::vector<shape>& in_shapes_container,
    const axis_aligned_box& in_node_bounds,
    const array_index in_current,
    bounding_interval_hierarchy& out_nodes
) {
    const ptrdiff_t shape_count = std::distance(in_shapes.begin, in_shapes.end);
    if (shape_count < 1)
    {
        return;
    }
    if (shape_count > 1)
    {
        std::vector<shape>::iterator middle = in_shapes.end;
        axis_aligned_box left_box = in_node_bounds;
        axis_aligned_box right_box = in_node_bounds;

        if (split(in_shapes, out_nodes[in_current], middle, left_box, right_box) != BIH_node_type::leaf)
        {
            out_nodes.push_back(BIH_node{ BIH_node_type::leaf });
            out_nodes.push_back(BIH_node{ BIH_node_type::leaf });
            out_nodes[in_current].children.left = out_nodes.size() - 2;
            out_nodes[in_current].children.right = out_nodes.size() - 1;
            make_hierarchy({ in_shapes.begin, middle }, in_shapes_container, left_box, out_nodes[in_current].children.left, out_nodes);
            make_hierarchy({ middle, in_shapes.end }, in_shapes_container, right_box, out_nodes[in_current].children.right, out_nodes);
            return;
        }
    }
    out_nodes[in_current].shape_group.index = std::distance(in_shapes_container.begin(), in_shapes.begin);
    out_nodes[in_current].shape_group.count = shape_count;
}

bounding_interval_hierarchy make_hierarchy(std::vector<shape>& in_shapes)
{
    if (in_shapes.empty())
    {
        return {};
    }

    bounding_interval_hierarchy nodes{ BIH_node{ BIH_node_type::leaf } };
    nodes.reserve(2 * in_shapes.size());
    make_hierarchy(iterator_pair{ in_shapes }, in_shapes, calculate_bounds(in_shapes), 0, nodes);
    nodes.shrink_to_fit();
    return nodes;
}