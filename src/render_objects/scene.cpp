#include <render_objects/scene.hpp>

#include <cstring>

std::vector<uint8_t> scene::to_bytes() const
{
    const size_t sky_size = sizeof(texture);

    const size_t shapes_size = sizeof(shape) * this->shapes.size();
    const size_t sphere_shapes_size = sizeof(sphere_shape) * this->sphere_shapes.size();
    const size_t plane_shapes_size = sizeof(plane_shape) * this->plane_shapes.size();
    const size_t rectangle_shapes_size = sizeof(rectangle_shape) * this->rectangle_shapes.size();
    const size_t box_shapes_size = sizeof(box_shape) * this->box_shapes.size();
    const size_t textured_box_shapes_size = sizeof(textured_box_shape) * this->textured_box_shapes.size();
    const size_t triangle_shapes_size = sizeof(triangle_shape) * this->triangle_shapes.size();
    const size_t meshes_size = sizeof(mesh) * this->meshes.size();

    const size_t dielectric_materials_size = sizeof(dielectric_material) * this->dielectric_materials.size();
    const size_t diffuse_light_materials_size = sizeof(diffuse_light_material) * this->diffuse_light_materials.size();
    const size_t lambertian_materials_size = sizeof(lambertian_material) * this->lambertian_materials.size();
    const size_t metal_materials_size = sizeof(metal_material) * this->metal_materials.size();

    const size_t checker_textures_size = sizeof(checker_texture) * this->checker_textures.size();
    const size_t constant_textures_size = sizeof(constant_texture) * this->constant_textures.size();
    const size_t image_textures_size = sizeof(image_texture) * this->image_textures.size();
    const size_t noise_textures_size = sizeof(noise_texture) * this->noise_textures.size();

    std::vector<uint8_t> bytes(sky_size
        + shapes_size
        + sphere_shapes_size
        + plane_shapes_size
        + rectangle_shapes_size
        + box_shapes_size
        + textured_box_shapes_size
        + triangle_shapes_size
        + meshes_size
        + dielectric_materials_size
        + diffuse_light_materials_size
        + lambertian_materials_size
        + metal_materials_size
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
    append_data(this->shapes.data(), shapes_size);
    append_data(this->sphere_shapes.data(), sphere_shapes_size);
    append_data(this->plane_shapes.data(), plane_shapes_size);
    append_data(this->rectangle_shapes.data(), rectangle_shapes_size);
    append_data(this->box_shapes.data(), box_shapes_size);
    append_data(this->textured_box_shapes.data(), textured_box_shapes_size);
    append_data(this->triangle_shapes.data(), triangle_shapes_size);
    append_data(this->meshes.data(), meshes_size);
    append_data(this->dielectric_materials.data(), dielectric_materials_size);
    append_data(this->diffuse_light_materials.data(), diffuse_light_materials_size);
    append_data(this->lambertian_materials.data(), lambertian_materials_size);
    append_data(this->metal_materials.data(), metal_materials_size);
    append_data(this->checker_textures.data(), checker_textures_size);
    append_data(this->constant_textures.data(), constant_textures_size);
    append_data(this->image_textures.data(), image_textures_size);
    append_data(this->noise_textures.data(), noise_textures_size);

    return bytes;
}

size_t scene::size() const
{
    const size_t sky_size = sizeof(texture);

    const size_t shapes_size = sizeof(shape) * this->shapes.size();
    const size_t sphere_shapes_size = sizeof(sphere_shape) * this->sphere_shapes.size();
    const size_t plane_shapes_size = sizeof(plane_shape) * this->plane_shapes.size();
    const size_t rectangle_shapes_size = sizeof(rectangle_shape) * this->rectangle_shapes.size();
    const size_t box_shapes_size = sizeof(box_shape) * this->box_shapes.size();
    const size_t textured_box_shapes_size = sizeof(textured_box_shape) * this->textured_box_shapes.size();
    const size_t triangle_shapes_size = sizeof(triangle_shape) * this->triangle_shapes.size();
    const size_t meshes_size = sizeof(mesh) * this->meshes.size();

    const size_t dielectric_materials_size = sizeof(dielectric_material) * this->dielectric_materials.size();
    const size_t diffuse_light_materials_size = sizeof(diffuse_light_material) * this->diffuse_light_materials.size();
    const size_t lambertian_materials_size = sizeof(lambertian_material) * this->lambertian_materials.size();
    const size_t metal_materials_size = sizeof(metal_material) * this->metal_materials.size();

    const size_t checker_textures_size = sizeof(checker_texture) * this->checker_textures.size();
    const size_t constant_textures_size = sizeof(constant_texture) * this->constant_textures.size();
    const size_t image_textures_size = sizeof(image_texture) * this->image_textures.size();
    const size_t noise_textures_size = sizeof(noise_texture) * this->noise_textures.size();

    return sky_size
        + shapes_size
        + sphere_shapes_size
        + plane_shapes_size
        + rectangle_shapes_size
        + box_shapes_size
        + textured_box_shapes_size
        + triangle_shapes_size
        + meshes_size
        + dielectric_materials_size
        + diffuse_light_materials_size
        + lambertian_materials_size
        + metal_materials_size
        + checker_textures_size
        + constant_textures_size
        + image_textures_size
        + noise_textures_size;
}

// Shapes

shape scene::add_shape(const sphere_shape& in_sphere, const material& in_material)
{
    this->shapes.push_back(shape{ shape_type::sphere, this->sphere_shapes.size(), in_material });
    this->sphere_shapes.push_back(in_sphere);
    return this->shapes.back();
}

shape scene::add_shape(const plane_shape& in_plane, const material& in_material)
{
    this->shapes.push_back(shape{ shape_type::plane, this->plane_shapes.size(), in_material });
    this->plane_shapes.push_back(in_plane);
    return this->shapes.back();
}

shape scene::add_shape(const rectangle_shape& in_rectangle, const material& in_material)
{
    this->shapes.push_back(shape{ shape_type::rectangle, this->rectangle_shapes.size(), in_material });
    this->rectangle_shapes.push_back(in_rectangle);
    return this->shapes.back();
}

shape scene::add_shape(const box_shape& in_box, const material& in_material)
{
    this->shapes.push_back(shape{ shape_type::box, this->box_shapes.size(), in_material });
    this->box_shapes.push_back(in_box);
    return this->shapes.back();
}

shape scene::add_shape(const textured_box_shape& in_textured_box, const material& in_material)
{
    this->shapes.push_back(shape{ shape_type::textured_box, this->textured_box_shapes.size(), in_material });
    this->textured_box_shapes.push_back(in_textured_box);
    return this->shapes.back();
}

shape scene::add_shape(const triangle_shape& in_triangle, const material& in_material)
{
    this->shapes.push_back(shape{ shape_type::triangle, this->triangle_shapes.size(), in_material });
    this->triangle_shapes.push_back(in_triangle);
    return this->shapes.back();
}

shape scene::add_shape(const mesh& in_mesh, const material& in_material)
{
    this->shapes.push_back(shape{ shape_type::mesh, this->meshes.size(), in_material });
    this->meshes.push_back(in_mesh);
    return this->shapes.back();
}

// Materials

material scene::add_material(const dielectric_material& in_material)
{
    this->dielectric_materials.push_back(in_material);
    return material{ material_type::dielectric, this->dielectric_materials.size() - 1 };
}

material scene::add_material(const diffuse_light_material& in_material)
{
    this->diffuse_light_materials.push_back(in_material);
    return material{ material_type::diffuse_light, this->diffuse_light_materials.size() - 1 };
}

material scene::add_material(const lambertian_material& in_material)
{
    this->lambertian_materials.push_back(in_material);
    return material{ material_type::lambertian, this->lambertian_materials.size() - 1 };
}

material scene::add_material(const metal_material& in_material)
{
    this->metal_materials.push_back(in_material);
    return material{ material_type::metal, this->metal_materials.size() - 1 };
}

// Textures

texture scene::add_texture(const checker_texture& in_texture)
{
    this->checker_textures.push_back(in_texture);
    return texture{ texture_type::checker, this->checker_textures.size() - 1 };
}

texture scene::add_texture(const constant_texture& in_texture)
{
    this->constant_textures.push_back(in_texture);
    return texture{ texture_type::constant, this->constant_textures.size() - 1 };
}

texture scene::add_texture(const image_texture& in_texture)
{
    this->image_textures.push_back(in_texture);
    return texture{ texture_type::image, this->image_textures.size() - 1 };
}

texture scene::add_texture(const noise_texture& in_texture)
{
    this->noise_textures.push_back(in_texture);
    return texture{ texture_type::noise, this->noise_textures.size() - 1 };
}