#pragma once

#include <render_objects/shapes.hpp>
#include <renderer_cpu/ray.hpp>
#include <util/geometric.hpp>
#include <util/pairs.hpp>
#include <util/vector_types.hpp>

struct hit_record
{
    float t;
    position_3d point;
    displacement_3d normal;
    material mat;
    uv_mapping texture_coord;
    bool occurred = true;

    static hit_record nope()
    {
        return hit_record{ 0.f, position_3d{ 0.f, 0.f, 0.f }, displacement_3d{ 0.f, 0.f, 0.f },
            material{ material_type::none, 0 }, uv_mapping{ 0.f, 0.f }, false };
    }
};

inline static hit_record ray_hits(const plane_shape& in_plane, const ray& in_ray, const min_max<float>& t)
{
    if (const float dot = glm::dot(in_plane.front, in_ray.direction); dot != 0.f)
    {
        const displacement_3d displacement = in_plane.origin - in_ray.origin;
        if (const float t1 = glm::dot(in_plane.front, displacement) / dot; t.is_value_clamped(t1))
        {
            const position_3d hit_point = in_ray.point_at_parameter(t1);
            return hit_record{ t1, hit_point, (-1.f + 2.f * float(dot < 0.f)) * in_plane.front, {}, /*texture_mapping*/ };
        }
    }
    return hit_record::nope();
}

inline static hit_record ray_hits(const sphere_shape& in_sphere, const ray& in_ray, const min_max<float>& t)
{
    const displacement_3d oc = in_ray.origin - in_sphere.origin;
    const float a = glm::dot(in_ray.direction, in_ray.direction);
    const float b = glm::dot(oc, in_ray.direction);
    const float c = glm::dot(oc, oc) - glm::pow(in_sphere.radius, 2);
    const float discriminant = b * b - a * c;

    if (discriminant > 0.f)
    {
        float root = (-b - glm::sqrt(discriminant)) / a;
        if (!t.is_value_clamped(root))
        {
            root = (-b + glm::sqrt(discriminant)) / a;
        }
        if (t.is_value_clamped(root))
        {
            const position_3d hit_point = in_ray.point_at_parameter(root);
            const uv_mapping texture_coord = uv_on_sphere((hit_point - in_sphere.origin) / in_sphere.radius, in_sphere.axial_tilt);
            return hit_record{ root, hit_point, (hit_point - in_sphere.origin) / in_sphere.radius, {}, texture_coord };
        }
    }
    return hit_record::nope();
}

inline static hit_record ray_hits(const triangle_shape& in_triangle, const ray& in_ray, const min_max<float>& t)
{
    // Möller-Trumbore algorithm
    const displacement_3d edge_1 = in_triangle.b - in_triangle.a;
    const displacement_3d edge_2 = in_triangle.c - in_triangle.a;
    const displacement_3d p_vec = glm::cross(in_ray.direction, edge_2);
    const float determinant = glm::dot(edge_1, p_vec);
    if (glm::abs(determinant) > glm::epsilon<float>())
    {
        const float inverse_determinant = 1.f / determinant;
        const displacement_3d t_vec = in_ray.origin - in_triangle.a;
        const displacement_3d q_vec = glm::cross(t_vec, edge_1);
        const float u = glm::dot(t_vec, p_vec) * inverse_determinant;
        const float v = glm::dot(in_ray.direction, q_vec) * inverse_determinant;
        if (u >= 0.f && u <= 1.f && v >= 0.f && u + v <= 1.f)
        {
            if (const float out_t = inverse_determinant * glm::dot(q_vec, edge_2); t.is_value_clamped(out_t))
            {
                const position_3d hit_point = in_ray.origin + (out_t * in_ray.direction);
                const direction_3d normal = ((1.f - u - v) * in_triangle.normal_a) + (v * in_triangle.normal_b) + (u * in_triangle.normal_c);
                return hit_record{ out_t, hit_point, normal, {}, uv_mapping{} };
            }
        }
    }
    return hit_record::nope();
}

inline static hit_record ray_hits(const scene& in_scene, const shape& in_shape, const ray& in_ray, const min_max<float>& t)
{
    auto hit = hit_record::nope();

    switch (in_shape.type)
    {
        case shape_type::plane:    hit = ray_hits(in_scene.plane_shapes[in_shape.index], in_ray, t); break;
        case shape_type::sphere:   hit = ray_hits(in_scene.sphere_shapes[in_shape.index], in_ray, t); break;
        case shape_type::triangle: hit = ray_hits(in_scene.triangle_shapes[in_shape.index], in_ray, t); break;
        default: break;
    }

    if (hit.occurred)
    {
        hit.mat = in_shape.mat;
    }
    return hit;
}