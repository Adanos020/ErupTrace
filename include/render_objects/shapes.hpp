#pragma once

#include <render_objects/materials.hpp>
#include <util/barycentric.hpp>
#include <util/geometric.hpp>
#include <util/numeric.hpp>
#include <util/sizes.hpp>
#include <util/vector.hpp>

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

struct plane_shape : plane
{
    min_max<position_3D> textured_area;

    axis_aligned_box bounding_box() const
    {
        return axis_aligned_box::zero();
    }
};

struct sphere_shape : sphere
{
    direction_3D axial_tilt;

    axis_aligned_box bounding_box() const
    {
        return axis_aligned_box::cube(this->origin, this->radius);
    }
};

struct triangle_shape : triangle
{
    direction_3D normal_a, normal_b, normal_c;
    barycentric_2D mapping_a, mapping_b, mapping_c;

    axis_aligned_box bounding_box() const
    {
        return axis_aligned_box{
            position_3D{
                std::min(std::min(this->a.x, this->b.x), this->c.x),
                std::min(std::min(this->a.y, this->b.y), this->c.y),
                std::min(std::min(this->a.z, this->b.z), this->c.z), },
            position_3D{
                std::max(std::max(this->a.x, this->b.x), this->c.x),
                std::max(std::max(this->a.y, this->b.y), this->c.y),
                std::max(std::max(this->a.z, this->b.z), this->c.z), }, };
    }
};