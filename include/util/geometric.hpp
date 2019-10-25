#pragma once

#include <util/pairs.hpp>
#include <util/sizes.hpp>
#include <util/vector_types.hpp>

#include <glm/gtc/constants.hpp>

#include <utility>

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

inline static uv_mapping uv_on_sphere(const position_3d& normalized_p)
{
    return {
        1.f - (glm::atan(normalized_p.z, normalized_p.x) + glm::pi<float>()) * glm::one_over_two_pi<float>(),
        (glm::asin(normalized_p.y) + glm::half_pi<float>()) * glm::one_over_pi<float>()
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

struct rectangle : plane
{
    extent_2d<float> size;

    static rectangle square(const position_3d& origin, const direction_3d& direction, const float size)
    {
        return rectangle{ plane{ origin, direction }, extent_2d<float>{ size, size } };
    }
};

struct cuboid : plane
{
    extent_3d<float> size;

    static cuboid cube(const position_3d& origin, const direction_3d& direction, const float size)
    {
        return cuboid{ plane{ origin, direction }, extent_3d<float>{ size, size, size } };
    }
};