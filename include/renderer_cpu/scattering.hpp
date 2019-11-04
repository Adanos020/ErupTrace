#pragma once

#include <renderer_cpu/ray.hpp>
#include <util/colors.hpp>

struct scattering
{
    color attenuation;
    ray scattered_ray;
    bool occurred = true;

    static scattering nope()
    {
        return scattering{ color{ 1.f }, ray{}, false };
    }
};

scattering scatter(const struct scene&, const struct material&, const ray&, const struct hit_record&);