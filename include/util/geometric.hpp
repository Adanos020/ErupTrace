#pragma once

#include <util/colors.hpp>
#include <util/pairs.hpp>
#include <util/sizes.hpp>
#include <util/vector_types.hpp>

#include <glm/gtc/constants.hpp>

#include <utility>

enum class axis_plane
{
    yz, xz, xy
};

struct line
{
    position_3d origin;
    direction_3d direction;

    position_3d point_at_parameter(const float t) const
    {
        return origin + t * direction;
    }
};

struct sphere
{
    position_3d origin;
    float radius;
};

inline static uv_mapping uv_on_sphere(const position_3d& in_normalized_p, const direction_3d& in_axial_tilt)
{
    return uv_mapping{
        1.f - (glm::atan(in_normalized_p.z, in_normalized_p.x) + glm::pi<float>()) * glm::one_over_two_pi<float>(),
        (glm::asin(in_normalized_p.y) + glm::half_pi<float>()) * glm::one_over_pi<float>(),
    };
}

struct plane
{
    position_3d origin;
    direction_3d front;

    plane inverse() const
    {
        return plane{ this->origin, -this->front };
    }
};

struct triangle
{
    position_3d a, b, c;
};

template<axis_plane ax>
struct plane_aligned_rectangle
{
    position_3d origin;
    extent_2d<float> size;

    static plane_aligned_rectangle<ax> square(const position_3d& origin, const direction_3d& direction, const float size)
    {
        return plane_aligned_rectangle<ax>{ origin, extent_2d<float>{ size, size } };
    }
};

using yz_aligned_rectangle = plane_aligned_rectangle<axis_plane::yz>;
using xz_aligned_rectangle = plane_aligned_rectangle<axis_plane::xz>;
using xy_aligned_rectangle = plane_aligned_rectangle<axis_plane::xy>;

struct axis_aligned_cuboid
{
    position_3d origin;
    extent_3d<float> size;

    static axis_aligned_cuboid cube(const position_3d& origin, const direction_3d& direction, const float size)
    {
        return axis_aligned_cuboid{ origin, extent_3d<float>{ size, size, size } };
    }
};