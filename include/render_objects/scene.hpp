#pragma once

#include <render_objects/hierarchy.hpp>
#include <render_objects/image.hpp>
#include <render_objects/materials.hpp>
#include <render_objects/shape_assembly.hpp>
#include <render_objects/shapes.hpp>
#include <render_objects/textures.hpp>

#include <vector>

struct scene
{
    texture sky;
    bounding_interval_hierarchy hierarchy;

    std::vector<uint8_t> to_bytes() const;
    size_t size() const;

    // Shapes

    std::vector<shape> infinite_shapes;
    std::vector<shape> shapes;
    std::vector<sphere_shape> sphere_shapes;
    std::vector<plane_shape> plane_shapes;
    std::vector<triangle_shape> triangle_shapes;

    shape add_plane_shape(const plane_shape&, const material&);
    shape add_plane_shape(const plane&, const material&);

    shape add_sphere_shape(const sphere_shape&, const material&);
    shape add_sphere_shape(const sphere&, const direction_3D& axial_tilt, const material&);
    
    shape add_triangle_shape(const triangle_shape&, const material&);
    shape add_triangle_shape(const triangle&, const std::array<direction_3D, 3>& normals,
        const std::array<barycentric_2D, 3>& texture_mappings, const material&);
    shape add_triangle_shape(const triangle&, const direction_3D& normal,
        const std::array<barycentric_2D, 3>& texture_mappings, const material&);
    
    void assemble_quad(const quad_assembly_info&);
    void assemble_cuboid(const cuboid_assembly_info&);

    // Materials

    std::vector<dielectric_material> dielectric_materials;
    std::vector<diffuse_material> diffuse_materials;
    std::vector<emit_light_material> emit_light_materials;
    std::vector<reflect_material> reflect_materials;

    material add_dielectric_material(const dielectric_material&, invalidable_array_index normal_map_index);
    material add_dielectric_material(float refractive_index, const texture& albedo, const texture& normals);
    material add_dielectric_material(float refractive_index, const texture& albedo);

    material add_diffuse_material(const diffuse_material&, invalidable_array_index normal_map_index);
    material add_diffuse_material(const texture& albedo, const texture& normals);
    material add_diffuse_material(const texture& albedo);

    material add_emit_light_material(const emit_light_material&, invalidable_array_index normal_map_index);
    material add_emit_light_material(float intensity, const texture& emit, const texture& normals);
    material add_emit_light_material(float intensity, const texture& emit);

    material add_reflect_material(const reflect_material&, invalidable_array_index normal_map_index);
    material add_reflect_material(float fuzz, const texture& albedo, const texture& normals);
    material add_reflect_material(float fuzz, const texture& albedo);

    // Textures

    std::vector<checker_texture> checker_textures;
    std::vector<constant_texture> constant_textures;
    std::vector<image_texture> image_textures;
    std::vector<noise_texture> noise_textures;
    std::vector<normal_texture> normal_textures;

    texture add_checker_texture(const checker_texture&);
    texture add_checker_texture(const scale_2D&, const color& odd, const color& even);

    texture add_constant_texture(const constant_texture&);
    texture add_constant_texture(const color&);

    texture add_image_texture(const image_texture&);
    texture add_image_texture(array_index image_index, wrap_method, filtering_method);
    texture add_image_texture(array_index image_index, const min_max<texture_position_2D>& image_fragment,
        wrap_method, filtering_method);
    
    texture add_noise_texture(const noise_texture&);
    texture add_noise_texture(const float&, const color&);

    texture add_normal_texture(const normal_texture&);
    texture add_normal_texture(array_index map_index, wrap_method, filtering_method);
    texture add_normal_texture(array_index map_index, const min_max<texture_position_2D>& map_fragment,
        wrap_method, filtering_method);

    // Assets

    std::vector<image> images;
    std::vector<normal_map> normal_maps;

    uint32_t add_image(const image&);
    uint32_t add_image(std::string_view path);
    uint32_t add_normal_map(const normal_map&);
    uint32_t add_normal_map(std::string_view path);
};