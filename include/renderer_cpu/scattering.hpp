#pragma once

#include <renderer_cpu/ray.hpp>
#include <util/colors.hpp>

struct scatter_record
{
    color albedo;
    ray scattered_ray;
    float PDF = 0.f;
    bool is_specular = true;
    bool occurred = true;

    static scatter_record nope()
    {
        return scatter_record{ color{ 1.f }, ray{}, 0.f, true, false };
    }
};

scatter_record scatter(const struct scene&, const struct material&, const ray&, const struct hit_record&);