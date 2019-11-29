#pragma once

#include <util/barycentric.hpp>
#include <util/vector.hpp>

struct vertex
{
    position_3D position;
    direction_3D normal;
    barycentric_2D mapping;
};