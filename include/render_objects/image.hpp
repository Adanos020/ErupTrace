#pragma once

#include <util/barycentric.hpp>
#include <util/colors.hpp>
#include <util/pairs.hpp>
#include <util/sizes.hpp>
#include <util/vector.hpp>

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
        catrom,
        cubic,
        hermite,
        linear,
        nearest,
    };

    extent_2D<uint32_t> size;
    std::vector<color> pixels;
};

color filter_catrom(const image&, const min_max<texture_position_2D>& fragment, const barycentric_2D& mapping, image::wrap_method);
color filter_cubic(const image&, const min_max<texture_position_2D>& fragment, const barycentric_2D& mapping, image::wrap_method);
color filter_hermite(const image&, const min_max<texture_position_2D>& fragment, const barycentric_2D& mapping, image::wrap_method);
color filter_linear(const image&, const min_max<texture_position_2D>& fragment, const barycentric_2D& mapping, image::wrap_method);
color filter_nearest(const image&, const min_max<texture_position_2D>& fragment, const barycentric_2D& mapping, image::wrap_method);