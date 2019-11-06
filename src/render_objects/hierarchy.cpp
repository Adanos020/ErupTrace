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

static axis_aligned_box calculate_bounds(const const_iterator_pair<std::vector<shape>>& in_shapes)
{
    axis_aligned_box scene_bounds = in_shapes.begin->bounding_box;

    std::for_each(std::next(in_shapes.begin), in_shapes.end, [&](const shape& s)
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

template<BIH_node::node_type component>
static void split(const iterator_pair<std::vector<shape>>& in_shapes, const float in_split_plane,
    const array_index in_current, bounding_interval_hierarchy& out_hierarchy, std::vector<shape>::iterator& out_middle,
    axis_aligned_box& out_left_box, axis_aligned_box& out_right_box)
{
    out_hierarchy[in_current].type = component;
    constexpr uint32_t comp = uint32_t(component);

    if (std::distance(in_shapes.begin, in_shapes.end) == 2 &&
        in_shapes.begin->bounding_box.origin() == std::next(in_shapes.begin)->bounding_box.origin())
    {
        out_middle = std::next(in_shapes.begin);
        out_hierarchy[in_current].clip.left = in_shapes.begin->bounding_box.max[comp];
        out_hierarchy[in_current].clip.right = out_middle->bounding_box.min[comp];
    }
    else
    {
        out_middle = std::partition(in_shapes.begin, in_shapes.end, [in_split_plane](const shape& a)
            { return a.bounding_box.origin()[comp] < in_split_plane; });

        if (out_middle != in_shapes.begin)
        {
            const auto compare_max = [](const shape& a, const shape& b) { return a.bounding_box.max[comp] < b.bounding_box.max[comp]; };
            const std::vector<shape>::iterator max_left = std::max_element(in_shapes.begin, out_middle, compare_max);
            out_hierarchy[in_current].clip.left = max_left->bounding_box.max[comp];
        }
        else
        {
            out_middle = std::next(in_shapes.begin);
            out_hierarchy[in_current].clip.left = infinity;
        }

        if (out_middle != in_shapes.end)
        {
            const auto compare_min = [](const shape& a, const shape& b) { return a.bounding_box.min[comp] < b.bounding_box.min[comp]; };
            const std::vector<shape>::iterator min_right = std::min_element(out_middle, in_shapes.end, compare_min);
            out_hierarchy[in_current].clip.right = min_right->bounding_box.min[comp];
        }
        else
        {
            out_middle = std::prev(in_shapes.end);
            out_hierarchy[in_current].clip.right = -infinity;
        }
    }

    out_left_box.max[comp] = out_hierarchy[in_current].clip.left;
    out_right_box.min[comp] = out_hierarchy[in_current].clip.right;
}

static void make_hierarchy(const iterator_pair<std::vector<shape>>& in_shapes, std::vector<shape>& in_shapes_container,
    bounding_interval_hierarchy& out_hierarchy, const axis_aligned_box& in_node_bounds, const array_index in_current)
{
    if (std::distance(in_shapes.begin, in_shapes.end) == 1)
    {
        out_hierarchy[in_current].type = BIH_node::node_type::leaf;
        out_hierarchy[in_current].shape_index = std::distance(in_shapes_container.begin(), in_shapes.begin);
    }
    else
    {
        std::vector<shape>::iterator middle = in_shapes.begin;
        axis_aligned_box left_box = in_node_bounds;
        axis_aligned_box right_box = in_node_bounds;

        const extent_3D<float> box_size = in_node_bounds.size();
        const position_3D box_center = in_node_bounds.origin();
        if (box_size.width > box_size.height && box_size.width > box_size.depth)
        {
            split<BIH_node::node_type::x>(in_shapes, box_center.x, in_current, out_hierarchy, middle, left_box, right_box);
        }
        else if (box_size.height > box_size.depth)
        {
            split<BIH_node::node_type::y>(in_shapes, box_center.y, in_current, out_hierarchy, middle, left_box, right_box);
        }
        else
        {
            split<BIH_node::node_type::z>(in_shapes, box_center.z, in_current, out_hierarchy, middle, left_box, right_box);
        }

        if (in_shapes.begin != middle)
        {
            const uint32_t left_index = out_hierarchy.size();
            out_hierarchy[in_current].children.left = left_index;
            out_hierarchy.push_back(BIH_node{});
            make_hierarchy({ in_shapes.begin, middle }, in_shapes_container, out_hierarchy, left_box, left_index);
        }
        if (in_shapes.end != middle)
        {
            const uint32_t right_index = out_hierarchy.size();
            out_hierarchy[in_current].children.left = right_index;
            out_hierarchy.push_back(BIH_node{});
            make_hierarchy({ middle, in_shapes.end }, in_shapes_container, out_hierarchy, right_box, right_index);
        }
    }
}

bounding_interval_hierarchy make_hierarchy(std::vector<shape>& in_shapes)
{
    if (in_shapes.empty()) { return {}; }

    bounding_interval_hierarchy hierarchy{ BIH_node{} };
    make_hierarchy(iterator_pair{ in_shapes }, in_shapes, hierarchy, calculate_bounds(in_shapes), 0);
    return hierarchy;
}

// Intersection

std::pair<bool, bool> line_intersects_children_of(const struct line& in_line, const BIH_node& in_node)
{
    return { false, false };
}