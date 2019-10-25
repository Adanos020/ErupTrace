#pragma once

#include <render_objects/materials.hpp>
#include <util/geometric.hpp>
#include <util/numeric_types.hpp>
#include <util/sizes.hpp>
#include <util/vector_types.hpp>

#include <optional>

enum class shape_type
{
    none, sphere, plane, rectangle, box, textured_box, triangle, mesh
};

struct shape
{
    shape_type type;
    array_index index;
    material mat;
};

struct sphere_shape : sphere
{
    rotator rotation;
};

struct plane_shape : plane
{
};

struct rectangle_shape : rectangle
{
};

struct box_shape : cuboid
{
};

struct textured_box_shape : cuboid
{
    texture front_texture;
    texture back_texture;
    texture top_texture;
    texture bottom_texture;
    texture left_texture;
    texture right_texture;
};

struct triangle_shape
{
};

struct mesh
{
};