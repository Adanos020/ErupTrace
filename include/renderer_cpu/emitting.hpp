#pragma once

#include <util/colors.hpp>
#include <renderer_cpu/hit.hpp>
#include <renderer_cpu/textures.hpp>
#include <render_objects/materials.hpp>

inline static color emit(const scene& in_scene, const emit_light_material& in_material, const hit_record& in_hit)
{
    return color_on_texture(in_scene, in_material.emit, in_hit.texture_coord, in_hit.point);
}

inline static color emit(const scene& in_scene, const material& in_material, const hit_record& in_hit)
{
    switch (in_material.type)
    {
        case material_type::emit_light: return emit(in_scene, in_scene.emit_light_materials[in_material.index], in_hit);
        default: break;
    }
    return black;
}