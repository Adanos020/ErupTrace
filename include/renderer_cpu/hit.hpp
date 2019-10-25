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

inline static hit_record ray_hits(const box_shape& in_box, const ray& in_ray, const min_max<float>& t)
{
    return hit_record::nope();
}

inline static hit_record ray_hits(const mesh& in_mesh, const ray& in_ray, const min_max<float>& t)
{
    return hit_record::nope();
}

inline static hit_record ray_hits(const plane_shape& in_plane, const ray& in_ray, const min_max<float>& t)
{
    if (const float dot = glm::dot(in_plane.front, in_ray.direction); dot != 0.f)
    {
        const displacement_3d displacement = in_plane.origin - in_ray.origin;
        if (const float t1 = glm::dot(in_plane.front, displacement) / dot; t1 > t.min && t1 < t.max)
        {
            const position_3d hit_point = in_ray.point_at_parameter(t1);
            return hit_record{ t1, hit_point, dot < 0.f ? in_plane.front : -in_plane.front, {}, /*texture_mapping*/ };
        }
    }
    return hit_record::nope();
}

inline static hit_record ray_hits(const rectangle_shape& in_rectangle, const ray& in_ray, const min_max<float>& t)
{
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
        if (root > t.max || root < t.min)
        {
            root = (-b + glm::sqrt(discriminant)) / a;
        }
        if (root < t.max && root > t.min)
        {
            const position_3d hit_point = in_ray.point_at_parameter(root);
            const uv_mapping texture_coord = uv_on_sphere((hit_point - in_sphere.origin) / in_sphere.radius);
            return hit_record{ root, hit_point, (hit_point - in_sphere.origin) / in_sphere.radius, {}, texture_coord };
        }
    }
    return hit_record::nope();
}

inline static hit_record ray_hits(const triangle_shape& in_triangle, const ray& in_ray, const min_max<float>& t)
{
    return hit_record::nope();
}

inline static hit_record ray_hits(const scene& in_scene, const shape& in_shape, const ray& in_ray, const min_max<float>& t)
{
    hit_record hit = hit_record::nope();

    switch (in_shape.type)
    {
        case shape_type::box:       hit = ray_hits(in_scene.box_shapes[in_shape.index], in_ray, t); break;
        case shape_type::mesh:      hit = ray_hits(in_scene.meshes[in_shape.index], in_ray, t); break;
        case shape_type::plane:     hit = ray_hits(in_scene.plane_shapes[in_shape.index], in_ray, t); break;
        case shape_type::rectangle: hit = ray_hits(in_scene.rectangle_shapes[in_shape.index], in_ray, t); break;
        case shape_type::sphere:    hit = ray_hits(in_scene.sphere_shapes[in_shape.index], in_ray, t); break;
        case shape_type::triangle:  hit = ray_hits(in_scene.triangle_shapes[in_shape.index], in_ray, t); break;
        default: break;
    }

    if (hit.occurred)
    {
        hit.mat = in_shape.mat;
    }
    return hit;
}