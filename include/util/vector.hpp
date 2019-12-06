#pragma once

#include <glm/glm.hpp>

using texture_position_3D = glm::vec3;
using position_3D = glm::vec3;
using displacement_3D = glm::vec3;
using direction_3D = glm::vec3;
using scale_3D = glm::vec3;

static constexpr direction_3D x_axis = { 1.f, 0.f, 0.f };
static constexpr direction_3D y_axis = { 0.f, 1.f, 0.f };
static constexpr direction_3D z_axis = { 0.f, 0.f, 1.f };

using texture_position_2D = glm::vec2;
using position_2D = glm::vec2;
using displacement_2D = glm::vec2;
using direction_2D = glm::vec2;
using scale_2D = glm::vec2;

using pixel_position = glm::ivec2;
using voxel_position = glm::ivec3;

static constexpr direction_2D up_2D = { 0.f, 1.f };
static constexpr direction_2D down_2D = { 0.f, -1.f };
static constexpr direction_2D left_2D = { 1.f, 0.f };
static constexpr direction_2D right_2D = { 1.f, 0.f };

class ortho_normal_base
{
public:
    constexpr ortho_normal_base(const direction_3D& in_w)
        : axis()
    {
        this->axis[2] = in_w;
        const glm::vec3 a = (glm::abs(in_w.x) > 0.9f) ? y_axis : x_axis;
        this->axis[1] = glm::normalize(glm::cross(in_w, a));
        this->axis[0] = glm::cross(in_w, this->v());
    }

    const glm::vec3& u() const { return this->axis[0]; }
    const glm::vec3& v() const { return this->axis[1]; }
    const glm::vec3& w() const { return this->axis[2]; }

    glm::vec3 local(const glm::vec3& in_v) const
    {
        return (in_v.x * this->u()) + (in_v.y * this->v()) + (in_v.z * this->w());
    }

private:
    direction_3D axis[3];
};

template <int32_t L, typename T, enum glm::qualifier Q = glm::packed_highp>
inline static glm::vec<L, T, Q> square_length(const glm::vec<L, T, Q>& in_vec)
{
    return glm::dot(in_vec, in_vec);
}

template <int32_t L, typename T, enum glm::qualifier Q = glm::packed_highp>
inline static glm::vec<L, T, Q> remove_NaNs(const glm::vec<L, T, Q>& in_vec)
{
    glm::vec<L, T, Q> ret_vec;
    for (int32_t i = 0; i < L; ++i)
    {
        ret_vec[i] = glm::isnan(in_vec[i]) ? T{ 0 } : in_vec[i];
    }
    return ret_vec;
}

inline static direction_3D map_normal(const direction_3D& in_world_normal, const direction_3D& in_mapped_normal)
{
    displacement_3D t = glm::cross(in_world_normal, y_axis);
    if (glm::dot(t, t) == 0)
    {
        t = glm::cross(in_world_normal, z_axis);
    }
    t = glm::normalize(t);
    const displacement_3D b = glm::normalize(glm::cross(in_world_normal, t));
    const glm::mat3 normal_transform = { t, b, in_world_normal };
    return glm::normalize(normal_transform * in_mapped_normal);
}