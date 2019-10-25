#pragma once

#include <render_objects/materials.hpp>
#include <render_objects/shapes.hpp>
#include <render_objects/textures.hpp>

#include <vector>

struct scene
{
    texture sky;

    std::vector<shape> shapes;
    std::vector<sphere_shape> sphere_shapes;
    std::vector<plane_shape> plane_shapes;
    std::vector<rectangle_shape> rectangle_shapes;
    std::vector<box_shape> box_shapes;
    std::vector<textured_box_shape> textured_box_shapes;
    std::vector<triangle_shape> triangle_shapes;
    std::vector<mesh> meshes;

    std::vector<dielectric_material> dielectric_materials;
    std::vector<diffuse_light_material> diffuse_light_materials;
    std::vector<lambertian_material> lambertian_materials;
    std::vector<metal_material> metal_materials;

    std::vector<checker_texture> checker_textures;
    std::vector<constant_texture> constant_textures;
    std::vector<image_texture> image_textures;
    std::vector<noise_texture> noise_textures;

    std::vector<uint8_t> to_bytes() const;
    size_t size() const;

    shape add_shape(const sphere_shape&, const material&);
    shape add_shape(const plane_shape&, const material&);
    shape add_shape(const rectangle_shape&, const material&);
    shape add_shape(const box_shape&, const material&);
    shape add_shape(const textured_box_shape&, const material&);
    shape add_shape(const triangle_shape&, const material&);
    shape add_shape(const mesh&, const material&);

    material add_material(const dielectric_material&);
    material add_material(const diffuse_light_material&);
    material add_material(const lambertian_material&);
    material add_material(const metal_material&);

    texture add_texture(const checker_texture&);
    texture add_texture(const constant_texture&);
    texture add_texture(const image_texture&);
    texture add_texture(const noise_texture&);
};