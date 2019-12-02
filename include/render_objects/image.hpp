#pragma once

#include <util/barycentric.hpp>
#include <util/colors.hpp>
#include <util/pairs.hpp>
#include <util/sizes.hpp>
#include <util/vector.hpp>

#include <vector>

struct vector_map
{
    extent_2D<uint32_t> size;
    std::vector<glm::vec3> pixels;
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

using image = vector_map;
using normal_map = vector_map;

#define FILTER_ARGS \
    const vector_map&, \
    const min_max<texture_position_2D>& map_fragment, \
    const barycentric_2D& mapping, \
    wrap_method
glm::vec3 filter_catrom(FILTER_ARGS);
glm::vec3 filter_linear(FILTER_ARGS);
glm::vec3 filter_nearest(FILTER_ARGS);
#undef FILTER_ARGS