#pragma once

#include <util/colors.hpp>
#include <util/pairs.hpp>
#include <util/sizes.hpp>
#include <util/vector_types.hpp>

#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>

#include <algorithm>
#include <utility>

enum class axis_plane
{
    yz, xz, xy
};

struct line
{
    position_3d origin;
    direction_3d direction;

    position_3d point_at_distance(const float t) const
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
    const direction_3d tilted = in_normalized_p * glm::rotation(y_axis, in_axial_tilt);
    return uv_mapping{
        1.f - (glm::atan(tilted.z, tilted.x) + glm::pi<float>()) * glm::one_over_two_pi<float>(),
        (glm::asin(tilted.y) + glm::half_pi<float>()) * glm::one_over_pi<float>(),
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

    static plane_aligned_rectangle<ax> square(const position_3d& in_origin, const float in_size)
    {
        return plane_aligned_rectangle<ax>{ in_origin, extent_2d<float>{ in_size, in_size } };
    }
};

using yz_aligned_rectangle = plane_aligned_rectangle<axis_plane::yz>;
using xz_aligned_rectangle = plane_aligned_rectangle<axis_plane::xz>;
using xy_aligned_rectangle = plane_aligned_rectangle<axis_plane::xy>;

struct axis_aligned_box
{
    position_3d min;
    position_3d max;

    static axis_aligned_box zero()
    {
        return axis_aligned_box{ position_3d{ 0.f }, position_3d{ 0.f } };
    }

    static axis_aligned_box cuboid(const position_3d& in_origin, const extent_3d<float> in_half_size)
    {
        return axis_aligned_box{
            in_origin - glm::vec3{ in_half_size.width, in_half_size.height, in_half_size.depth },
            in_origin + glm::vec3{ in_half_size.width, in_half_size.height, in_half_size.depth },
        };
    }

    static axis_aligned_box cuboid(const line& in_diagonal, const float in_length)
    {
        const position_3d a = in_diagonal.origin;
        const position_3d b = in_diagonal.point_at_distance(in_length);
        return axis_aligned_box{
            position_3d{ std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z) },
            position_3d{ std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z) },
        };
    }

    static axis_aligned_box cube(const position_3d& in_origin, const float in_half_size)
    {
        return cuboid(in_origin, { in_half_size, in_half_size, in_half_size });
    }
};