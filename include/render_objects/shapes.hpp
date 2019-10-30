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
};

struct sphere_shape : sphere
{
    direction_3d axial_tilt;
};

struct plane_shape : plane
{
    min_max<position_3d> textured_area;
};

struct triangle_shape : triangle
{
    direction_3d normal_a, normal_b, normal_c;
    uv_mapping uv_a, uv_b, uv_c;
};