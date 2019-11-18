#pragma once

#include <util/barycentric.hpp>
#include <util/colors.hpp>
#include <util/pairs.hpp>
#include <util/sizes.hpp>
#include <util/vector.hpp>

#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>

#include <algorithm>
#include <utility>

struct line
{
    position_3D origin;
    direction_3D direction;

    position_3D point_at_distance(const float t) const
    {
        return origin + t * direction;
    }
};

struct sphere
{
    position_3D origin;
    float radius;
};

inline static barycentric_2D mapping_on_sphere(const position_3D& in_normalized_p, const direction_3D& in_axial_tilt)
{
    const direction_3D tilted = in_normalized_p * glm::rotation(y_axis, in_axial_tilt);
    return barycentric_2D{
        1.f - ((glm::atan(tilted.z, tilted.x) + glm::pi<float>()) * glm::one_over_two_pi<float>()),
        1.f - ((glm::asin(tilted.y) + glm::half_pi<float>()) * glm::one_over_pi<float>()),
    };
}

struct plane
{
    position_3D origin;
    direction_3D front;

    plane inverse() const
    {
        return plane{ this->origin, -this->front };
    }
};

struct triangle
{
    position_3D a, b, c;
};

struct axis_aligned_box
{
    position_3D min;
    position_3D max;

    float width() const
    {
        return max.x - min.x;
    }

    float height() const
    {
        return max.y - min.y;
    }

    float depth() const
    {
        return max.z - min.z;
    }

    extent_3D<float> size() const
    {
        return { this->width(), this->height(), this->depth() };
    }

    position_3D origin() const
    {
        return this->min + displacement_3D{ this->width() * 0.5f, this->height() * 0.5f, this->depth() * 0.5f };
    }

    static axis_aligned_box zero()
    {
        return axis_aligned_box{ position_3D{ 0.f }, position_3D{ 0.f } };
    }

    static axis_aligned_box cuboid(const position_3D& in_origin, const extent_3D<float> in_half_size)
    {
        return axis_aligned_box{
            in_origin - displacement_3D{ in_half_size.width, in_half_size.height, in_half_size.depth },
            in_origin + displacement_3D{ in_half_size.width, in_half_size.height, in_half_size.depth }, };
    }

    static axis_aligned_box cuboid(const line& in_diagonal, const float in_length)
    {
        const position_3D a = in_diagonal.origin;
        const position_3D b = in_diagonal.point_at_distance(in_length);
        return axis_aligned_box{
            position_3D{ std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z) },
            position_3D{ std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z) }, };
    }

    static axis_aligned_box cube(const position_3D& in_origin, const float in_half_size)
    {
        return cuboid(in_origin, { in_half_size, in_half_size, in_half_size });
    }
};