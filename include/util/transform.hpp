#pragma once

#include <util/vector_types.hpp>

struct rotator
{
    float pitch;
    float yaw;
    float roll;

    rotator& operator+=(const rotator&);
    rotator& operator-=(const rotator&);
};

inline static rotator operator+(const rotator& a, const rotator& b)
{
    return { a.pitch + b.pitch, a.yaw + b.yaw, a.roll + b.roll };
}

inline static rotator operator-(const rotator& a, const rotator& b)
{
    return { a.pitch - b.pitch, a.yaw - b.yaw, a.roll - b.roll };
}

inline static rotator operator-(const rotator& a)
{
    return { -a.pitch, -a.yaw, -a.roll };
}

struct transform_3d
{
    position_3D translation = { 0.f, 0.f, 0.f };
    glm::vec3 scale = { 1.f, 1.f, 1.f };
    rotator rotation = { 0.f, 0.f, 0.f };

    transform_3d& move_to(const position_3D&);
    transform_3d& move_by(const displacement_3D&);
    transform_3d& scale_to(const glm::vec3&);
    transform_3d& scale_to(float scalar);
    transform_3d& scale_by(const glm::vec3&);
    transform_3d& scale_by(float scalar);
    transform_3d& rotate_to(const rotator&);
    transform_3d& rotate_by(const rotator&);
};

struct transform_2d
{
    position_2D translation = { 0.f, 0.f };
    glm::vec2 scale = { 1.f, 1.f };
    float rotation = 0.f;

    transform_2d& move_to(const position_2D&);
    transform_2d& move_by(const displacement_2D&);
    transform_2d& scale_to(const glm::vec2&);
    transform_2d& scale_to(float scalar);
    transform_2d& scale_by(const glm::vec2&);
    transform_2d& scale_by(float scalar);
    transform_2d& rotate_to(float angle);
    transform_2d& rotate_by(float angle);
};