#include <renderer_cpu/textures.hpp>

#include <render_objects/image.hpp>
#include <render_objects/scene.hpp>
#include <render_objects/textures.hpp>
#include <util/interpolation.hpp>
#include <util/numeric.hpp>
#include <util/vector.hpp>

static color color_on_texture(const checker_texture& in_checker_texture, const UV_mapping& in_UV)
{
    if (glm::sin(in_checker_texture.transform.translation.x + in_checker_texture.transform.scale.x * in_UV.U) *
        glm::sin(in_checker_texture.transform.translation.y + in_checker_texture.transform.scale.y * in_UV.V) < 0.f)
    {
        return in_checker_texture.odd;
    }
    return in_checker_texture.even;
}

static color color_on_texture(const constant_texture& in_constant_texture)
{
    return in_constant_texture.value;
}

static color color_on_texture(const scene& in_scene, const image_texture& in_image_texture, const UV_mapping& in_UV)
{
    UV_mapping final_UV = in_UV;
    if (wrap(final_UV, in_image_texture.wrap))
    {
        return filter(in_scene.images[in_image_texture.image_index], final_UV, in_image_texture.filtering);
    }
    return black;
}

static color color_on_texture(const noise_texture& in_noise_texture, const UV_mapping& in_UV, const position_3D& in_position)
{
    return white;
}

color color_on_texture(const scene& in_scene, const texture& in_texture, const UV_mapping& in_UV, const position_3D& in_position)
{
    switch (in_texture.type)
    {
        case texture_type::checker:  return color_on_texture(in_scene.checker_textures[in_texture.index], in_UV);
        case texture_type::constant: return color_on_texture(in_scene.constant_textures[in_texture.index]);
        case texture_type::image:    return color_on_texture(in_scene, in_scene.image_textures[in_texture.index], in_UV);
        case texture_type::noise:    return color_on_texture(in_scene.noise_textures[in_texture.index], in_UV, in_position);
        default: break;
    }
    return black;
}