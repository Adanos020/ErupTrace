#pragma once

#include <util/colors.hpp>
#include <util/geometric.hpp>
#include <util/pairs.hpp>

struct ray : line
{
    const displacement_3d inverse_direction;
    const float time;

    ray();
    ray(const line&, float time = 0.f);
    static ray shoot(const struct camera&, const uv_mapping&);
    color trace(const struct scene&, int32_t depth = 50) const;
};