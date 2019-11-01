#pragma once

#include <render_objects/materials.hpp>
#include <util/geometric.hpp>
#include <util/numeric_types.hpp>
#include <util/sizes.hpp>
#include <util/vector_types.hpp>

#include <array>

enum class shape_type
{
    none, sphere, plane, triangle
};

struct shape
{
    shape_type type;
    array_index index;
    material mat;
    axis_aligned_box bounding_box;
};

struct sphere_shape : sphere
{
    direction_3d axial_tilt;

    axis_aligned_box bounding_box() const
    {
        return axis_aligned_box::cube(this->origin, this->radius);
    }
};

struct plane_shape : plane
{
    min_max<position_3d> textured_area;

    axis_aligned_box bounding_box() const
    {
        if (glm::abs(this->front) == x_axis)
        {
            return axis_aligned_box::cuboid(this->origin, { 0.f, FLT_MAX, FLT_MAX });
        }
        if (glm::abs(this->front) == y_axis)
        {
            return axis_aligned_box::cuboid(this->origin, { FLT_MAX, 0.f, FLT_MAX });
        }
        if (glm::abs(this->front) == z_axis)
        {
            return axis_aligned_box::cuboid(this->origin, { FLT_MAX, FLT_MAX, 0.f });
        }
        return axis_aligned_box::cube(this->origin, FLT_MAX);
    }
};

struct triangle_shape : triangle
{
    direction_3d normal_a, normal_b, normal_c;
    uv_mapping uv_a, uv_b, uv_c;

    axis_aligned_box bounding_box() const
    {
        return axis_aligned_box{
            position_3d{
                std::min(std::min(this->a.x, this->b.x), this->c.x),
                std::min(std::min(this->a.y, this->b.y), this->c.y),
                std::min(std::min(this->a.z, this->b.z), this->c.z),
            },
            position_3d{
                std::max(std::max(this->a.x, this->b.x), this->c.x),
                std::max(std::max(this->a.y, this->b.y), this->c.y),
                std::max(std::max(this->a.z, this->b.z), this->c.z),
            },
        };
    }
};