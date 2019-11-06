#include <util/transform.hpp>

// 3D

rotator& rotator::operator+=(const rotator& b)
{
    this->pitch += b.pitch;
    this->yaw += b.yaw;
    this->roll += b.roll;
    return *this;
}

rotator& rotator::operator-=(const rotator& b)
{
    this->pitch -= b.pitch;
    this->yaw -= b.yaw;
    this->roll -= b.roll;
    return *this;
}

transform_3d& transform_3d::move_to(const position_3D& position)
{
    this->translation = position;
    return *this;
}

transform_3d& transform_3d::move_by(const displacement_3D& displacement)
{
    this->translation += displacement;
    return *this;
}

transform_3d& transform_3d::scale_to(const glm::vec3& scale)
{
    this->scale = scale;
    return *this;
}

transform_3d& transform_3d::scale_to(const float scale)
{
    this->scale = glm::vec3{ scale };
    return *this;
}

transform_3d& transform_3d::scale_by(const glm::vec3& scale)
{
    this->scale *= scale;
    return *this;
}

transform_3d& transform_3d::scale_by(const float scale)
{
    this->scale *= scale;
    return *this;
}

transform_3d& transform_3d::rotate_to(const rotator& rotation)
{
    this->rotation = rotation;
    return *this;
}

transform_3d& transform_3d::rotate_by(const rotator& rotation)
{
    this->rotation += rotation;
    return *this;
}

// 2D

transform_2d& transform_2d::move_to(const position_2D& position)
{
    this->translation = position;
    return *this;
}

transform_2d& transform_2d::move_by(const displacement_2D& displacement)
{
    this->translation += displacement;
    return *this;
}

transform_2d& transform_2d::scale_to(const glm::vec2& scale)
{
    this->scale = scale;
    return *this;
}

transform_2d& transform_2d::scale_to(const float scale)
{
    this->scale = glm::vec3{ scale };
    return *this;
}

transform_2d& transform_2d::scale_by(const glm::vec2& scale)
{
    this->scale *= scale;
    return *this;
}

transform_2d& transform_2d::scale_by(const float scale)
{
    this->scale *= scale;
    return *this;
}

transform_2d& transform_2d::rotate_to(const float rotation)
{
    this->rotation = rotation;
    return *this;
}

transform_2d& transform_2d::rotate_by(const float rotation)
{
    this->rotation += rotation;
    return *this;
}