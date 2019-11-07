#include <render_objects/scene.hpp>

#include <cstring>

std::vector<uint8_t> scene::to_bytes() const
{
    const size_t sky_size = sizeof(texture);

    const size_t hierarchy_box_size = sizeof(axis_aligned_box);
    const size_t hierarchy_nodes_size = sizeof(BIH_node) * this->hierarchy.nodes.size();

    const size_t infinite_shapes_size = sizeof(shape) * this->infinite_shapes.size();
    const size_t shapes_size = sizeof(shape) * this->shapes.size();
    const size_t sphere_shapes_size = sizeof(sphere_shape) * this->sphere_shapes.size();
    const size_t plane_shapes_size = sizeof(plane_shape) * this->plane_shapes.size();
    const size_t triangle_shapes_size = sizeof(triangle_shape) * this->triangle_shapes.size();

    const size_t dielectric_materials_size = sizeof(dielectric_material) * this->dielectric_materials.size();
    const size_t diffuse_materials_size = sizeof(diffuse_material) * this->diffuse_materials.size();
    const size_t emit_light_materials_size = sizeof(emit_light_material) * this->emit_light_materials.size();
    const size_t reflect_materials_size = sizeof(reflect_material) * this->reflect_materials.size();

    const size_t checker_textures_size = sizeof(checker_texture) * this->checker_textures.size();
    const size_t constant_textures_size = sizeof(constant_texture) * this->constant_textures.size();
    const size_t image_textures_size = sizeof(image_texture) * this->image_textures.size();
    const size_t noise_textures_size = sizeof(noise_texture) * this->noise_textures.size();

    std::vector<uint8_t> bytes(sky_size
        + hierarchy_box_size
        + hierarchy_nodes_size
        + infinite_shapes_size
        + shapes_size
        + sphere_shapes_size
        + plane_shapes_size
        + triangle_shapes_size
        + dielectric_materials_size
        + diffuse_materials_size
        + emit_light_materials_size
        + reflect_materials_size
        + checker_textures_size
        + constant_textures_size
        + image_textures_size
        + noise_textures_size, 0);

    size_t current_position = 0;
    const auto append_data = [&](auto* source, const size_t size)
    {
        std::memcpy(bytes.data() + current_position, source, size);
        current_position += size;
    };
    append_data(&this->sky, sky_size);
    append_data(&this->hierarchy.bounding_box, hierarchy_box_size);
    append_data(this->hierarchy.nodes.data(), hierarchy_nodes_size);
    append_data(this->infinite_shapes.data(), infinite_shapes_size);
    append_data(this->shapes.data(), shapes_size);
    append_data(this->sphere_shapes.data(), sphere_shapes_size);
    append_data(this->plane_shapes.data(), plane_shapes_size);
    append_data(this->triangle_shapes.data(), triangle_shapes_size);
    append_data(this->dielectric_materials.data(), dielectric_materials_size);
    append_data(this->diffuse_materials.data(), diffuse_materials_size);
    append_data(this->emit_light_materials.data(), emit_light_materials_size);
    append_data(this->reflect_materials.data(), reflect_materials_size);
    append_data(this->checker_textures.data(), checker_textures_size);
    append_data(this->constant_textures.data(), constant_textures_size);
    append_data(this->image_textures.data(), image_textures_size);
    append_data(this->noise_textures.data(), noise_textures_size);

    return bytes;
}

size_t scene::size() const
{
    const size_t sky_size = sizeof(texture);

    const size_t hierarchy_box_size = sizeof(axis_aligned_box);
    const size_t hierarchy_nodes_size = sizeof(BIH_node) * this->hierarchy.nodes.size();

    const size_t infinite_shapes_size = sizeof(shape) * this->infinite_shapes.size();
    const size_t shapes_size = sizeof(shape) * this->shapes.size();
    const size_t sphere_shapes_size = sizeof(sphere_shape) * this->sphere_shapes.size();
    const size_t plane_shapes_size = sizeof(plane_shape) * this->plane_shapes.size();
    const size_t triangle_shapes_size = sizeof(triangle_shape) * this->triangle_shapes.size();

    const size_t dielectric_materials_size = sizeof(dielectric_material) * this->dielectric_materials.size();
    const size_t diffuse_materials_size = sizeof(diffuse_material) * this->diffuse_materials.size();
    const size_t emit_light_materials_size = sizeof(emit_light_material) * this->emit_light_materials.size();
    const size_t reflect_materials_size = sizeof(reflect_material) * this->reflect_materials.size();

    const size_t checker_textures_size = sizeof(checker_texture) * this->checker_textures.size();
    const size_t constant_textures_size = sizeof(constant_texture) * this->constant_textures.size();
    const size_t image_textures_size = sizeof(image_texture) * this->image_textures.size();
    const size_t noise_textures_size = sizeof(noise_texture) * this->noise_textures.size();

    return sky_size
        + hierarchy_box_size
        + hierarchy_nodes_size
        + infinite_shapes_size
        + shapes_size
        + sphere_shapes_size
        + plane_shapes_size
        + triangle_shapes_size
        + dielectric_materials_size
        + emit_light_materials_size
        + diffuse_materials_size
        + reflect_materials_size
        + checker_textures_size
        + constant_textures_size
        + image_textures_size
        + noise_textures_size;
}

// Shapes

shape scene::add_plane_shape(const plane_shape& in_plane, const material& in_material)
{
    this->infinite_shapes.push_back(shape{ shape_type::plane, this->plane_shapes.size(), in_material, axis_aligned_box::zero() });
    this->plane_shapes.push_back(in_plane);
    return this->infinite_shapes.back();
}

shape scene::add_plane_shape(const plane& in_plane, const min_max<position_3D>& in_textured_area, const material& in_material)
{
    return this->add_plane_shape(plane_shape{ in_plane, in_textured_area }, in_material);
}

shape scene::add_sphere_shape(const sphere_shape& in_sphere, const material& in_material)
{
    this->shapes.push_back(shape{ shape_type::sphere, this->sphere_shapes.size(), in_material, in_sphere.bounding_box() });
    this->sphere_shapes.push_back(in_sphere);
    return this->shapes.back();
}

shape scene::add_sphere_shape(const sphere& in_sphere, const direction_3D& in_axial_tilt, const material& in_material)
{
    return this->add_sphere_shape(sphere_shape{ in_sphere, in_axial_tilt }, in_material);
}

shape scene::add_triangle_shape(const triangle_shape& in_triangle, const material& in_material)
{
    this->shapes.push_back(shape{ shape_type::triangle, this->triangle_shapes.size(), in_material, in_triangle.bounding_box() });
    this->triangle_shapes.push_back(in_triangle);
    return this->shapes.back();
}

shape scene::add_triangle_shape(const triangle& in_triangle, const std::array<direction_3D, 3>& in_normals,
    const std::array<UV_mapping, 3>& in_uv_mappings, const material& in_material)
{
    return this->add_triangle_shape(triangle_shape{ in_triangle,
        in_normals[0], in_normals[1], in_normals[2],
        in_uv_mappings[0], in_uv_mappings[1], in_uv_mappings[2] }, in_material);
}

shape scene::add_triangle_shape(const triangle& in_triangle, const direction_3D& in_normal,
    const std::array<UV_mapping, 3>& in_uv_mappings, const material& in_material)
{
    return this->add_triangle_shape(triangle_shape{ in_triangle,
        in_normal, in_normal, in_normal,
        in_uv_mappings[0], in_uv_mappings[1], in_uv_mappings[2] }, in_material);
}

// Materials

material scene::add_dielectric_material(const dielectric_material& in_material)
{
    this->dielectric_materials.push_back(in_material);
    return material{ material_type::dielectric, this->dielectric_materials.size() - 1 };
}

material scene::add_dielectric_material(const float in_refractive_index, const texture& in_albedo)
{
    return this->add_dielectric_material(dielectric_material{ in_refractive_index, in_albedo });
}

material scene::add_diffuse_material(const diffuse_material& in_material)
{
    this->diffuse_materials.push_back(in_material);
    return material{ material_type::diffuse, this->diffuse_materials.size() - 1 };
}

material scene::add_diffuse_material(const texture& in_albedo)
{
    return this->add_diffuse_material(diffuse_material{ in_albedo });
}

material scene::add_emit_light_material(const emit_light_material& in_material)
{
    this->emit_light_materials.push_back(in_material);
    return material{ material_type::emit_light, this->emit_light_materials.size() - 1 };
}

material scene::add_emit_light_material(const texture& in_emit)
{
    return this->add_emit_light_material(emit_light_material{ in_emit });
}

material scene::add_reflect_material(const reflect_material& in_material)
{
    this->reflect_materials.push_back(in_material);
    return material{ material_type::reflect, this->reflect_materials.size() - 1 };
}

material scene::add_reflect_material(const float in_fuzz, const texture& in_albedo)
{
    return this->add_reflect_material(reflect_material{ in_fuzz, in_albedo });
}

// Textures

texture scene::add_checker_texture(const checker_texture& in_texture)
{
    this->checker_textures.push_back(in_texture);
    return texture{ texture_type::checker, this->checker_textures.size() - 1 };
}

texture scene::add_checker_texture(const transform_2d& in_transform, const color& in_odd, const color& in_even)
{
    return this->add_checker_texture(checker_texture{ in_transform, in_odd, in_even });
}

texture scene::add_constant_texture(const constant_texture& in_texture)
{
    this->constant_textures.push_back(in_texture);
    return texture{ texture_type::constant, this->constant_textures.size() - 1 };
}

texture scene::add_constant_texture(const color& in_color)
{
    return this->add_constant_texture(constant_texture{ in_color });
}

texture scene::add_image_texture(const image_texture& in_texture)
{
    this->image_textures.push_back(in_texture);
    return texture{ texture_type::image, this->image_textures.size() - 1 };
}

texture scene::add_image_texture(const array_index in_index, const min_max<UV_mapping>& in_uv_mapping,
    const extent_2D<uint32_t>& in_image_size, const image_texture::wrap_method in_wrap,
    const image_texture::filtering_method in_filtering)
{
    return this->add_image_texture(image_texture{ in_index, in_uv_mapping, in_image_size, in_wrap, in_filtering });
}

texture scene::add_noise_texture(const noise_texture& in_texture)
{
    this->noise_textures.push_back(in_texture);
    return texture{ texture_type::noise, this->noise_textures.size() - 1 };
}

texture scene::add_noise_texture(const transform_2d& in_transform, const color& in_color)
{
    return this->add_noise_texture(noise_texture{ in_transform, in_color });
}