#include <renderer_cpu/scattering.hpp>

#include <render_objects/materials.hpp>
#include <render_objects/scene.hpp>
#include <renderer_cpu/hit.hpp>
#include <renderer_cpu/textures.hpp>
#include <util/random.hpp>

#include <glm/glm.hpp>

static float Schlick(const float in_cosine, const float in_refractive_index)
{
    const float r0 = glm::pow((1 - in_refractive_index) / (1 + in_refractive_index), 2);
    return r0 + (1 - r0) * glm::pow(1 - in_cosine, 5);
}

static scattering scatter(const scene& in_scene, const dielectric_material& in_dielectric, const ray& in_ray, const hit_record& in_hit)
{
    const float direction_dot_normal = glm::dot(in_ray.direction, in_hit.normal);
    const float direction_length = glm::length(in_ray.direction);

    const auto [outward_normal, eta, cosine] = direction_dot_normal > 0.f
        ? std::make_tuple(-in_hit.normal, in_dielectric.refractive_index, in_dielectric.refractive_index * direction_dot_normal / direction_length)
        : std::make_tuple(in_hit.normal, 1.f / in_dielectric.refractive_index, -direction_dot_normal / direction_length);

    const displacement_3D refracted = glm::refract(glm::normalize(in_ray.direction), outward_normal, eta);

    const float reflect_probability = refracted != displacement_3D{ 0.f }
        ? Schlick(cosine, in_dielectric.refractive_index) : 1.f;

    const color col = color_on_texture(in_scene, in_dielectric.albedo, in_hit.texture_coord, in_hit.point);
    if (random_chance(reflect_probability))
    {
        const displacement_3D reflected = glm::reflect(in_ray.direction, in_hit.normal);
        return scattering{ col, ray{ line{ in_hit.point, reflected }, in_ray.time } };
    }
    return scattering{ col, ray{ line{ in_hit.point, refracted }, in_ray.time } };
}

static scattering scatter(const scene& in_scene, const diffuse_material& in_diffuse, const ray& in_ray, const hit_record& in_hit)
{
    const position_3D target = in_hit.point + in_hit.normal + random_direction();
    return scattering{
        color_on_texture(in_scene, in_diffuse.albedo, in_hit.texture_coord, in_hit.point),
        ray{ line{ in_hit.point, target - in_hit.point }, in_ray.time },
    };
}

static scattering scatter(const scene& in_scene, const emit_light_material& in_emit_light, const ray& in_ray, const hit_record& in_hit)
{
    return scattering::nope();
}

static scattering scatter(const scene& in_scene, const reflect_material& in_reflect, const ray& in_ray, const hit_record& in_hit)
{
    const displacement_3D reflected = glm::reflect(glm::normalize(in_ray.direction), in_hit.normal);
    const ray scattered = { line{ in_hit.point, reflected + (in_reflect.fuzz * random_direction()) }, in_ray.time };
    if (glm::dot(scattered.direction, in_hit.normal) > 0.f)
    {
        return scattering{ color_on_texture(in_scene, in_reflect.albedo, in_hit.texture_coord, in_hit.point), scattered };
    }
    return scattering::nope();
}

scattering scatter(const scene& in_scene, const material& in_material, const ray& in_ray, const hit_record& in_hit)
{
    switch (in_material.type)
    {
        case material_type::dielectric: return scatter(in_scene, in_scene.dielectric_materials[in_material.index], in_ray, in_hit);
        case material_type::emit_light: return scatter(in_scene, in_scene.emit_light_materials[in_material.index], in_ray, in_hit);
        case material_type::diffuse:    return scatter(in_scene, in_scene.diffuse_materials[in_material.index], in_ray, in_hit);
        case material_type::reflect:    return scatter(in_scene, in_scene.reflect_materials[in_material.index], in_ray, in_hit);
        default: break;
    }
    return scattering::nope();
}