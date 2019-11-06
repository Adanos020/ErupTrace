#pragma once

#include <render_objects/shapes.hpp>
#include <renderer_cpu/ray.hpp>
#include <util/geometric.hpp>
#include <util/pairs.hpp>
#include <util/vector_types.hpp>

#include <vector>

struct hit_record
{
    float distance;
    position_3D point;
    displacement_3D normal;
    material mat;
    UV_mapping texture_coord;
    bool occurred = true;

    static hit_record nope()
    {
        return hit_record{ 0.f, position_3D{ 0.f, 0.f, 0.f }, displacement_3D{ 0.f, 0.f, 0.f },
            material{ material_type::none, 0 }, UV_mapping{ 0.f, 0.f }, false };
    }
};

hit_record ray_hits_anything(const struct scene&, const ray&);