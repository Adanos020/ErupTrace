#include <renderer_cpu/textures.hpp>

#include <render_objects/scene.hpp>
#include <render_objects/textures.hpp>

static color color_on_texture(const scene& in_scene, const checker_texture& in_texture, const uv_mapping& in_uv, const position_3d& in_position)
{
    if (glm::sin(in_texture.transform.translation.x + in_texture.transform.scale.x * in_uv.u) *
        glm::sin(in_texture.transform.translation.y + in_texture.transform.scale.y * in_uv.v) < 0.f)
    {
        return in_texture.odd;
    }
    return in_texture.even;
}

static color color_on_texture(const scene& in_scene, const constant_texture& in_texture, const uv_mapping& in_uv, const position_3d& in_position)
{
    return in_texture.value;
}

static color color_on_texture(const scene& in_scene, const image_texture& in_texture, const uv_mapping& in_uv, const position_3d& in_position)
{
    return white;
}

static color color_on_texture(const scene& in_scene, const noise_texture& in_texture, const uv_mapping& in_uv, const position_3d& in_position)
{
    return white;
}

color color_on_texture(const scene& in_scene, const texture& in_texture, const uv_mapping& in_uv, const position_3d& in_position)
{
    switch (in_texture.type)
    {
        case texture_type::checker:  return color_on_texture(in_scene, in_scene.checker_textures[in_texture.index], in_uv, in_position);
        case texture_type::constant: return color_on_texture(in_scene, in_scene.constant_textures[in_texture.index], in_uv, in_position);
        case texture_type::image:    return color_on_texture(in_scene, in_scene.image_textures[in_texture.index], in_uv, in_position);
        case texture_type::noise:    return color_on_texture(in_scene, in_scene.noise_textures[in_texture.index], in_uv, in_position);
        default: break;
    }
    return black;
}