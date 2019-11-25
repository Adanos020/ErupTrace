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