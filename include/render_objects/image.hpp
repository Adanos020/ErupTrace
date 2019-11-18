#pragma once

#include <util/barycentric.hpp>
#include <util/colors.hpp>
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

bool wrap(barycentric_2D& in_mapping, image::wrap_method);
color filter(const image&, const barycentric_2D&, image::filtering_method);