#include <renderer_cpu/textures.hpp>

#include <render_objects/scene.hpp>
#include <render_objects/textures.hpp>
#include <util/barycentric.hpp>
#include <util/interpolation.hpp>
#include <util/numeric.hpp>
#include <util/vector.hpp>

static color color_on_texture(const checker_texture& in_checker_texture, const barycentric_2D& in_mapping)
{
    if (glm::sin(in_checker_texture.scale.x * in_mapping.U) *
        glm::sin(in_checker_texture.scale.y * in_mapping.V) < 0.f)
    {
        return in_checker_texture.odd;
    }
    return in_checker_texture.even;
}

static color color_on_texture(const constant_texture& in_constant_texture)
{
    return in_constant_texture.value;
}

static glm::vec3 value_on_vector_map(const vector_map& in_map, const barycentric_2D& in_mapping,
    const min_max<texture_position_2D>& in_map_fragment, const wrap_method in_wrap_method,
    const filtering_method in_filtering_method)
{
    switch (in_filtering_method)
    {
        case filtering_method::catrom:  return filter_catrom(in_map, in_map_fragment, in_mapping, in_wrap_method);
        case filtering_method::linear:  return filter_linear(in_map, in_map_fragment, in_mapping, in_wrap_method);
        case filtering_method::nearest: return filter_nearest(in_map, in_map_fragment, in_mapping, in_wrap_method);
    }
}

static color color_on_texture(const scene& in_scene, const image_texture& in_image_texture, const barycentric_2D& in_mapping)
{
    return value_on_vector_map(
        in_scene.images[in_image_texture.image_index],
        in_mapping,
        in_image_texture.image_fragment,
        in_image_texture.wrap,
        in_image_texture.filtering);
}

static color color_on_texture(const noise_texture& in_noise_texture, const barycentric_2D& in_mapping, const position_3D& in_position)
{
    return white;
}

color color_on_texture(const scene& in_scene, const texture& in_texture, const barycentric_2D& in_mapping, const position_3D& in_position)
{
    switch (in_texture.type)
    {
        case texture_type::checker:  return color_on_texture(in_scene.checker_textures[in_texture.index], in_mapping);
        case texture_type::constant: return color_on_texture(in_scene.constant_textures[in_texture.index]);
        case texture_type::image:    return color_on_texture(in_scene, in_scene.image_textures[in_texture.index], in_mapping);
        case texture_type::noise:    return color_on_texture(in_scene.noise_textures[in_texture.index], in_mapping, in_position);
        default: return black;
    }
}

direction_3D normal_on_texture(const scene& in_scene, const normal_texture& in_normal_texture, const barycentric_2D& in_mapping)
{
    return value_on_vector_map(
        in_scene.normal_maps[in_normal_texture.map_index],
        in_mapping,
        in_normal_texture.map_fragment,
        in_normal_texture.wrap,
        in_normal_texture.filtering);
}