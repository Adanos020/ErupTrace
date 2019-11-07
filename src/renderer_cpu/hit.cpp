#include <renderer_cpu/hit.hpp>

#include <render_objects/scene.hpp>

#include <stack>

static hit_record ray_hits(const plane_shape& in_plane, const ray& in_ray, const min_max<float>& in_distances)
{
    if (const float dot = glm::dot(in_plane.front, in_ray.direction); dot != 0.f)
    {
        const displacement_3D displacement = in_plane.origin - in_ray.origin;
        if (const float distance = glm::dot(in_plane.front, displacement) / dot; in_distances.is_value_clamped(distance))
        {
            const position_3D hit_point = in_ray.point_at_distance(distance);
            return hit_record{ distance, hit_point, (-1.f + 2.f * float(dot < 0.f)) * in_plane.front, {}, /*texture_mapping*/ };
        }
    }
    return hit_record::nope();
}

static hit_record ray_hits(const sphere_shape& in_sphere, const ray& in_ray, const min_max<float>& in_distances)
{
    const displacement_3D oc = in_ray.origin - in_sphere.origin;
    const float a = glm::dot(in_ray.direction, in_ray.direction);
    const float b = glm::dot(oc, in_ray.direction);
    const float c = glm::dot(oc, oc) - glm::pow(in_sphere.radius, 2);
    const float discriminant = b * b - a * c;

    if (discriminant > 0.f)
    {
        float root = (-b - glm::sqrt(discriminant)) / a;
        if (!in_distances.is_value_clamped(root))
        {
            root = (-b + glm::sqrt(discriminant)) / a;
        }
        if (in_distances.is_value_clamped(root))
        {
            const position_3D hit_point = in_ray.point_at_distance(root);
            const UV_mapping texture_coord = uv_on_sphere((hit_point - in_sphere.origin) / in_sphere.radius, in_sphere.axial_tilt);
            return hit_record{ root, hit_point, (hit_point - in_sphere.origin) / in_sphere.radius, {}, texture_coord };
        }
    }
    return hit_record::nope();
}

static hit_record ray_hits(const triangle_shape& in_triangle, const ray& in_ray, const min_max<float>& in_distances)
{
    // Möller-Trumbore algorithm
    const displacement_3D edge_1 = in_triangle.b - in_triangle.a;
    const displacement_3D edge_2 = in_triangle.c - in_triangle.a;
    const displacement_3D p_vec = glm::cross(in_ray.direction, edge_2);
    const float determinant = glm::dot(edge_1, p_vec);
    if (glm::abs(determinant) > glm::epsilon<float>())
    {
        const float inverse_determinant = 1.f / determinant;
        const displacement_3D t_vec = in_ray.origin - in_triangle.a;
        const displacement_3D q_vec = glm::cross(t_vec, edge_1);
        const float u = glm::dot(t_vec, p_vec) * inverse_determinant;
        const float v = glm::dot(in_ray.direction, q_vec) * inverse_determinant;
        if (u >= 0.f && u <= 1.f && v >= 0.f && u + v <= 1.f)
        {
            if (const float distance = glm::dot(q_vec, edge_2) * inverse_determinant; in_distances.is_value_clamped(distance))
            {
                const position_3D hit_point = in_ray.point_at_distance(distance);
                const direction_3D normal = ((1.f - u - v) * in_triangle.normal_a) + (u * in_triangle.normal_b) + (v * in_triangle.normal_c);
                const UV_mapping texture_mapping = {
                    ((1.f - u - v) * in_triangle.uv_a.u) + (u * in_triangle.uv_b.u) + (v * in_triangle.uv_c.u),
                    ((1.f - u - v) * in_triangle.uv_a.v) + (u * in_triangle.uv_b.v) + (v * in_triangle.uv_c.v),
                };
                return hit_record{ distance, hit_point, normal, {}, texture_mapping };
            }
        }
    }
    return hit_record::nope();
}

static hit_record ray_hits(const scene& in_scene, const shape& in_shape, const ray& in_ray, min_max<float>& in_distances)
{
    auto hit = hit_record::nope();
    switch (in_shape.type)
    {
        case shape_type::plane:    hit = ray_hits(in_scene.plane_shapes[in_shape.index], in_ray, in_distances); break;
        case shape_type::sphere:   hit = ray_hits(in_scene.sphere_shapes[in_shape.index], in_ray, in_distances); break;
        case shape_type::triangle: hit = ray_hits(in_scene.triangle_shapes[in_shape.index], in_ray, in_distances); break;
        default: break;
    }
    if (hit.occurred)
    {
        hit.mat = in_shape.mat;
        in_distances.max = hit.distance;
    }
    return hit;
}

static std::pair<bool, bool> ray_hits_children_of(const ray& in_ray, const BIH_node& in_node,
    const axis_aligned_box& in_hierarchy_box, min_max<float>& out_distances)
{
    const uint32_t splitting_axis = uint32_t(in_node.type);

    axis_aligned_box left_box = in_hierarchy_box;
    axis_aligned_box right_box = in_hierarchy_box;
    left_box.max[splitting_axis] = in_node.clip.left;
    right_box.min[splitting_axis] = in_node.clip.right;

    const float distances_to_splitting_planes[2] = {
        (in_node.clip.left - in_ray.origin[splitting_axis]) * in_ray.inverse_direction[splitting_axis],
        (in_node.clip.right - in_ray.origin[splitting_axis]) * in_ray.inverse_direction[splitting_axis],
    };

    const size_t first = size_t(in_ray.direction[splitting_axis] < 0);
    const size_t second = 1 - first;

    bool hit_left = false, hit_right = false;
    if (distances_to_splitting_planes[first] > out_distances.min)
    {
        out_distances.max = std::min(out_distances.max, distances_to_splitting_planes[first]);
        hit_left = true;
    }
    if (distances_to_splitting_planes[second] < out_distances.max)
    {
        out_distances.min = std::max(out_distances.min, distances_to_splitting_planes[second]);
        hit_right = true;
    }

    return { hit_left, hit_right };
}

hit_record ray_hits_anything(const scene& in_scene, const ray& in_ray)
{
    hit_record closest_hit = hit_record::nope();

    min_max<float> distances = { 0.0001f, infinity };
    for (const shape& it_shape : in_scene.infinite_shapes)
    {
        if (const hit_record hit = ray_hits(in_scene, it_shape, in_ray, distances); hit.occurred)
        {
            closest_hit = hit;
        }
    }

    if (!in_scene.hierarchy.nodes.empty())
    {
        std::stack<BIH_node> node_stack;
        node_stack.push(in_scene.hierarchy.nodes.front());
        while (!node_stack.empty())
        {
            BIH_node current_node = node_stack.top();
            node_stack.pop();

            bool leaf_hit = true;
            while (current_node.type != BIH_node_type::leaf)
            {
                const auto [left_hit, right_hit] = ray_hits_children_of(in_ray, current_node, in_scene.hierarchy.bounding_box, distances);
                if (left_hit)
                {
                    current_node = in_scene.hierarchy.nodes[current_node.children.left];
                    if (right_hit)
                    {
                        node_stack.push(in_scene.hierarchy.nodes[current_node.children.right]);
                    }
                }
                else if (right_hit)
                {
                    current_node = in_scene.hierarchy.nodes[current_node.children.right];
                }
                else
                {
                    leaf_hit = false;
                    break;
                }
            }

            if (leaf_hit)
            {
                const shape& it_shape = in_scene.shapes[current_node.shape_index];
                if (const hit_record hit = ray_hits(in_scene, it_shape, in_ray, distances); hit.occurred)
                {
                    closest_hit = hit;
                }
            }
        }
    }

    return closest_hit;
}