#pragma once

#include <glm/glm.hpp>

using position_3d = glm::vec3;
using displacement_3d = glm::vec3;
using direction_3d = glm::vec3;

static constexpr direction_3d x_axis = { 1.f, 0.f, 0.f };
static constexpr direction_3d y_axis = { 0.f, 1.f, 0.f };
static constexpr direction_3d z_axis = { 0.f, 0.f, 1.f };

using position_2d = glm::vec2;
using displacement_2d = glm::vec2;
using direction_2d = glm::vec2;

static constexpr direction_2d up_2d = { 0.f, 1.f };
static constexpr direction_2d down_2d = { 0.f, -1.f };
static constexpr direction_2d left_2d = { 1.f, 0.f };
static constexpr direction_2d right_2d = { 1.f, 0.f };

enum class vector_component
{
    x,
    y,
    z,
    xyz,
};