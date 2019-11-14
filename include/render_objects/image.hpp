#pragma once

#include <util/colors.hpp>
#include <util/pairs.hpp>
#include <util/sizes.hpp>

#include <vector>

struct image
{
    enum class wrap_method
    {
        clamp_to_edge,
        clamp_to_border,
        mirrored_repeat,
        repeat,
    };
    
    enum class filtering_method
    {
        cubic,
        linear,
        nearest,
    };

    extent_2D<uint32_t> size;
    std::vector<color> pixels;
};

bool wrap(UV_mapping& in_UV, image::wrap_method);
color filter(const image&, const UV_mapping&, image::filtering_method);