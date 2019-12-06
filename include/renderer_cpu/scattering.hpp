#pragma once

#include <renderer_cpu/ray.hpp>
#include <util/colors.hpp>
//#include <util/density_functions.hpp>

struct scatter_record
{
    color albedo;
    ray scattered_ray;
    bool is_specular;
    float PDF = 1.f;
    float material_PDF = 1.f;
    bool occurred = true;

    static scatter_record nope()
    {
        return scatter_record{ color{ 1.f }, ray{}, true, 1.f, 1.f, false };
    }
};

scatter_record scatter(const struct scene&, const struct material&, const ray&, const struct hit_record&);