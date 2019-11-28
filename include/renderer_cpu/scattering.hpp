#pragma once

#include <renderer_cpu/ray.hpp>
#include <util/colors.hpp>

struct scattering_record
{
    color attenuation;
    ray scattered_ray;
    bool occurred = true;

    static scattering_record nope()
    {
        return scattering_record{ color{ 1.f }, ray{}, false };
    }
};

scattering_record scatter(const struct scene&, const struct material&, const ray&, const struct hit_record&);