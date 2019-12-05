#pragma once

#include <renderer_cpu/ray.hpp>
#include <util/colors.hpp>

struct scattering_record
{
    color albedo;
    ray scattered_ray;
    float pdf = 1.f;
    float material_pdf = 1.f;
    bool occurred = true;

    static scattering_record nope()
    {
        return scattering_record{ color{ 1.f }, ray{}, 1.f, 1.f, false };
    }
};

scattering_record scatter(const struct scene&, const struct material&, const ray&, const struct hit_record&);