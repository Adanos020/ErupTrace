#pragma once

#include <render_objects/textures.hpp>
#include <util/numeric.hpp>

enum class material_type
{
    none,
    dielectric,
    diffuse,
    emit_light,
    reflect,
};

struct material
{
    material_type type;
    array_index index;
    invalidable_array_index normal_texture_index = -1;
};

struct dielectric_material
{
    float refractive_index;
    texture albedo;
};

struct diffuse_material
{
    texture albedo;
};

struct emit_light_material
{
    texture emit;
    float intensity;
};

struct reflect_material
{
    float fuzz;
    texture albedo;
};