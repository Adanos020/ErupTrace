#pragma once

#include <render_objects/hierarchy.hpp>
#include <render_objects/image.hpp>
#include <render_objects/materials.hpp>
#include <render_objects/shapes.hpp>
#include <render_objects/shape_assembly.hpp>
#include <render_objects/textures.hpp>

#include <vector>

struct scene
{
    texture sky;

    bounding_interval_hierarchy hierarchy;

    std::vector<shape> infinite_shapes;
    std::vector<shape> shapes;
    std::vector<sphere_shape> sphere_shapes;
    std::vector<plane_shape> plane_shapes;
    std::vector<triangle_shape> triangle_shapes;

    std::vector<dielectric_material> dielectric_materials;
    std::vector<emit_light_material> emit_light_materials;
    std::vector<diffuse_material> diffuse_materials;
    std::vector<reflect_material> reflect_materials;

    std::vector<checker_texture> checker_textures;
    std::vector<constant_texture> constant_textures;
    std::vector<image_texture> image_textures;
    std::vector<noise_texture> noise_textures;

    std::vector<image> images;

    std::vector<uint8_t> to_bytes() const;
    size_t size() const;

    shape add_plane_shape(const plane_shape&, const material&);
    shape add_plane_shape(const plane&, const min_max<position_3D>& textured_area, const material&);
    shape add_sphere_shape(const sphere_shape&, const material&);
    shape add_sphere_shape(const sphere&, const direction_3D& axial_tilt, const material&);
    shape add_triangle_shape(const triangle_shape&, const material&);
    shape add_triangle_shape(const triangle&, const std::array<direction_3D, 3>& normals, const std::array<barycentric_2D, 3>&, const material&);
    shape add_triangle_shape(const triangle&, const direction_3D& normal, const std::array<barycentric_2D, 3>&, const material&);
    
    void assemble_quad(const quad_assembly_info&);
    void assemble_cuboid(const cuboid_assembly_info&);

    material add_dielectric_material(const dielectric_material&);
    material add_dielectric_material(float refractive_index, const texture& albedo);
    material add_diffuse_material(const diffuse_material&);
    material add_diffuse_material(const texture& albedo);
    material add_emit_light_material(const emit_light_material&);
    material add_emit_light_material(const texture& emit, float intensity);
    material add_reflect_material(const reflect_material&);
    material add_reflect_material(float fuzz, const texture& albedo);

    texture add_checker_texture(const checker_texture&);
    texture add_checker_texture(const transform_2d&, const color& odd, const color& even);
    texture add_constant_texture(const constant_texture&);
    texture add_constant_texture(const color&);
    texture add_image_texture(const image_texture&);
    texture add_image_texture(array_index, const min_max<texture_position_2D>& in_image_fragment, image::wrap_method, image::filtering_method);
    texture add_noise_texture(const noise_texture&);
    texture add_noise_texture(const transform_2d&, const color&);

    uint32_t add_image(const image&);
    uint32_t add_image(std::string_view path);
};