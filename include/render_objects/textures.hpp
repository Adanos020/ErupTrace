#pragma once

#include <util/colors.hpp>
#include <util/numeric_types.hpp>
#include <util/pairs.hpp>
#include <util/transform.hpp>
#include <util/sizes.hpp>

enum class texture_type
{
    none, checker, constant, image, noise
};

struct texture
{
    texture_type type;
    array_index index;
};

struct checker_texture
{
    transform_2d transform;
    color odd;
    color even;
};

struct constant_texture
{
    color value;
};

struct image_texture
{
    enum class wrap_method
    {
        repeat, mirrored_repeat, clamp_to_edge, clamp_to_border
    };

    enum class filtering_method
    {
        nearest, linear, anisotropic
    };

    array_index image_index;
    min_max<uv_mapping> image_fragment;
    extent_2d<uint32_t> image_size;
    transform_2d transform;
    wrap_method wrap;
    filtering_method filtering;
};

struct noise_texture
{
    transform_2d transform;
    color base_color;
};