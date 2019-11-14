#pragma once

#include <glm/glm.hpp>

using position_3D = glm::vec3;
using displacement_3D = glm::vec3;
using direction_3D = glm::vec3;

static constexpr direction_3D x_axis = { 1.f, 0.f, 0.f };
static constexpr direction_3D y_axis = { 0.f, 1.f, 0.f };
static constexpr direction_3D z_axis = { 0.f, 0.f, 1.f };

using position_2D = glm::vec2;
using displacement_2D = glm::vec2;
using direction_2D = glm::vec2;

static constexpr direction_2D up_2d = { 0.f, 1.f };
static constexpr direction_2D down_2d = { 0.f, -1.f };
static constexpr direction_2D left_2d = { 1.f, 0.f };
static constexpr direction_2D right_2d = { 1.f, 0.f };