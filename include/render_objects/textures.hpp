#pragma once

#include <render_objects/image.hpp>
#include <util/colors.hpp>
#include <util/numeric.hpp>
#include <util/pairs.hpp>
#include <util/sizes.hpp>

enum class texture_type
{
    none,
    checker,
    constant,
    image,
    noise,
};

struct texture
{
    texture_type type;
    array_index index;
};

struct checker_texture
{
    scale_2D scale;
    color odd, even;
};

struct constant_texture
{
    color value;
};

struct image_texture
{
    array_index image_index;
    min_max<texture_position_2D> image_fragment;
    image::wrap_method wrap;
    image::filtering_method filtering;
};

struct noise_texture
{
    float scale;
    color color_1, color_2;
};