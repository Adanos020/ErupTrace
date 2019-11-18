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
            const barycentric_2D texture_coord = mapping_on_sphere(
                (hit_point - in_sphere.origin) / in_sphere.radius, in_sphere.axial_tilt);
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
                const barycentric_2D texture_mapping = {
                    ((1.f - u - v) * in_triangle.mapping_a.U) + (u * in_triangle.mapping_b.U) + (v * in_triangle.mapping_c.U),
                    ((1.f - u - v) * in_triangle.mapping_a.V) + (u * in_triangle.mapping_b.V) + (v * in_triangle.mapping_c.V),
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

static auto ray_hits_children_of(const ray& in_ray, const BIH_node& in_node, const min_max<float>& in_distances)
{
    const uint32_t axis = uint32_t(in_node.type);
    const float distances_to_splitting_planes[2] = {
        (in_node.clip.left - in_ray.origin[axis]) * in_ray.inverse_direction[axis],
        (in_node.clip.right - in_ray.origin[axis]) * in_ray.inverse_direction[axis],
    };
    const size_t node_1 = size_t(in_ray.direction[axis] < 0);
    const size_t node_2 = 1 - node_1;

    struct
    {
        bool occurred = false;
        bool is_left_node;
        min_max<float> distances;
    } hit_1, hit_2;

    if (distances_to_splitting_planes[node_1] > in_distances.min)
    {
        hit_1.occurred = true;
        hit_1.is_left_node = node_1 == 0;
        hit_1.distances = { in_distances.min, std::min(in_distances.max, distances_to_splitting_planes[node_1]) };
    }
    if (distances_to_splitting_planes[node_2] < in_distances.max)
    {
        hit_2.occurred = true;
        hit_2.is_left_node = node_2 == 0;
        hit_2.distances = { std::max(in_distances.min, distances_to_splitting_planes[node_2]), in_distances.max };
    }

    return std::make_pair(hit_1, hit_2);
}

hit_record ray_hits_anything(const scene& in_scene, const ray& in_ray)
{
    hit_record closest_hit = hit_record::nope();
    min_max<float> distances = { 0.0001f, infinity<float> };

    for (const shape& it_shape : in_scene.infinite_shapes)
    {
        if (const hit_record hit = ray_hits(in_scene, it_shape, in_ray, distances); hit.occurred)
        {
            closest_hit = hit;
        }
    }

    if (!in_scene.hierarchy.empty())
    {
        struct stack_entry { BIH_node node; min_max<float> distances; };
        std::stack<stack_entry> node_stack;
        node_stack.push({ in_scene.hierarchy.front(), distances });
        while (!node_stack.empty())
        {
            stack_entry current_entry = node_stack.top();
            node_stack.pop();
            bool leaf_hit = true;
            while (current_entry.node.type != BIH_node_type::leaf)
            {
                uint32_t node_1 = current_entry.node.children.left;
                uint32_t node_2 = current_entry.node.children.right;
                const auto [hit_1, hit_2] = ray_hits_children_of(in_ray, current_entry.node, current_entry.distances);
                if (!hit_1.is_left_node)
                {
                    std::swap(node_1, node_2);
                }

                if (hit_1.occurred)
                {
                    current_entry = { in_scene.hierarchy[node_1], hit_1.distances };
                    if (hit_2.occurred)
                    {
                        node_stack.push({ in_scene.hierarchy[node_2], hit_2.distances });
                    }
                }
                else if (hit_2.occurred)
                {
                    current_entry = { in_scene.hierarchy[node_2], hit_2.distances };
                }
                else
                {
                    leaf_hit = false;
                    break;
                }
            }

            if (leaf_hit) for (uint32_t i = 0; i < current_entry.node.shape_group.count; ++i)
            {
                const shape& it_shape = in_scene.shapes[current_entry.node.shape_group.index + i];
                if (const hit_record hit = ray_hits(in_scene, it_shape, in_ray, distances); hit.occurred)
                {
                    closest_hit = hit;
                }
            }
        }
    }

    return closest_hit;
}