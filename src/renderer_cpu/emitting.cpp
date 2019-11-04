#include <renderer_cpu/emitting.hpp>

#include <render_objects/scene.hpp>
#include <renderer_cpu/textures.hpp>

static color emit(const scene& in_scene, const emit_light_material& in_material, const hit_record& in_hit)
{
    return color_on_texture(in_scene, in_material.emit, in_hit.texture_coord, in_hit.point);
}

color emit(const scene& in_scene, const material& in_material, const hit_record& in_hit)
{
    switch (in_material.type)
    {
        case material_type::emit_light: return emit(in_scene, in_scene.emit_light_materials[in_material.index], in_hit);
        default: break;
    }
    return black;
}