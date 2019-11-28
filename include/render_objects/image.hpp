#pragma once

#include <util/barycentric.hpp>
#include <util/colors.hpp>
#include <util/pairs.hpp>
#include <util/sizes.hpp>
#include <util/vector.hpp>

#include <vector>

template<typename vector_type>
struct vector_map
{
    extent_2D<uint32_t> size;
    std::vector<vector_type> pixels;
};

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
    linear,
    nearest,
};

using image = vector_map<color>;
using normal_map = vector_map<displacement_3D>;

#define FILTER_ARGS\
    const vector_map<vector_type>&,\
    const min_max<texture_position_2D>& map_fragment,\
    const barycentric_2D& mapping,\
    wrap_method
template<typename vector_type> vector_type filter_catrom(FILTER_ARGS);
template<typename vector_type> vector_type filter_linear(FILTER_ARGS);
template<typename vector_type> vector_type filter_nearest(FILTER_ARGS);
#undef FILTER_ARGS