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

static void split(
    const BIH_node_type in_split_axis,
    const iterator_pair<std::vector<shape>>& in_shapes,
    const float in_split_plane,
    const array_index in_current,
    std::vector<BIH_node>& out_nodes,
    std::vector<shape>::iterator& out_middle,
    axis_aligned_box& out_left_box,
    axis_aligned_box& out_right_box
) {
    out_nodes[in_current].type = in_split_axis;
    const uint32_t axis = uint32_t(in_split_axis);

    out_middle = std::partition(in_shapes.begin, in_shapes.end, [in_split_plane, axis](const shape& a)
        { return a.bounding_box.origin()[axis] < in_split_plane; });

    if (out_middle != in_shapes.begin)
    {
        const auto compare_max = [axis](const shape& a, const shape& b) { return a.bounding_box.max[axis] < b.bounding_box.max[axis]; };
        const std::vector<shape>::iterator max_left = std::max_element(in_shapes.begin, out_middle, compare_max);
        out_left_box.max[axis] = out_nodes[in_current].clip.left = max_left->bounding_box.max[axis];
    }
    else
    {
        out_middle = std::next(in_shapes.begin);
        out_nodes[in_current].clip.left = -infinity;
    }

    if (out_middle != in_shapes.end)
    {
        const auto compare_min = [axis](const shape& a, const shape& b) { return a.bounding_box.min[axis] < b.bounding_box.min[axis]; };
        const std::vector<shape>::iterator min_right = std::min_element(out_middle, in_shapes.end, compare_min);
        out_right_box.min[axis] = out_nodes[in_current].clip.right = min_right->bounding_box.min[axis];
    }
    else
    {
        out_middle = std::prev(in_shapes.end);
        out_nodes[in_current].clip.right = infinity;
    }
}

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

static void make_hierarchy(
    const iterator_pair<std::vector<shape>>& in_shapes,
    std::vector<shape>& in_shapes_container,
    const axis_aligned_box& in_node_bounds,
    const array_index in_current,
    std::vector<BIH_node>& out_nodes
) {
    if (const ptrdiff_t size = std::distance(in_shapes.begin, in_shapes.end); size < 2)
    {
        if (size == 1)
        {
            out_nodes[in_current].type = BIH_node_type::leaf;
            out_nodes[in_current].shape_index = std::distance(in_shapes_container.begin(), in_shapes.begin);
        }
    }
    else
    {
        std::vector<shape>::iterator middle = in_shapes.begin;
        axis_aligned_box left_box = in_node_bounds;
        axis_aligned_box right_box = in_node_bounds;

        const position_3D box_center = in_node_bounds.origin();
        const BIH_node_type split_axis = choose_split_axis(in_node_bounds);
        const uint32_t axis = uint32_t(split_axis);
        split(split_axis, in_shapes, box_center[axis], in_current, out_nodes, middle, left_box, right_box);

        if (std::isfinite(out_nodes[in_current].clip.left))
        {
            const uint32_t left_index = out_nodes.size();
            out_nodes[in_current].children.left = left_index;
            out_nodes.push_back(BIH_node{});
            make_hierarchy({ in_shapes.begin, middle }, in_shapes_container, left_box, left_index, out_nodes);
        }

        if (std::isfinite(out_nodes[in_current].clip.right))
        {
            const uint32_t right_index = out_nodes.size();
            out_nodes[in_current].children.right = right_index;
            out_nodes.push_back(BIH_node{});
            make_hierarchy({ middle, in_shapes.end }, in_shapes_container, right_box, right_index, out_nodes);
        }
    }
}

bounding_interval_hierarchy make_hierarchy(std::vector<shape>& in_shapes)
{
    if (in_shapes.empty()) { return bounding_interval_hierarchy{ axis_aligned_box::zero(), {} }; }

    const axis_aligned_box bounding_box = calculate_bounds(in_shapes);
    
    std::vector<BIH_node> nodes{ BIH_node{} };
    nodes.reserve(2 * in_shapes.size() - 1);
    make_hierarchy(iterator_pair{ in_shapes }, in_shapes, bounding_box, 0, nodes);
    nodes.shrink_to_fit();

    return bounding_interval_hierarchy{ bounding_box, nodes };
}