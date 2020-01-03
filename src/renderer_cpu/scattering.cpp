#include <renderer_cpu/scattering.hpp>

#include <render_objects/materials.hpp>
#include <render_objects/scene.hpp>
#include <renderer_cpu/hit.hpp>
#include <renderer_cpu/textures.hpp>
#include <util/random.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/optimum_pow.hpp>

static float Schlick(const float in_cosine, const float in_refractive_index)
{
    const float r_0 = glm::pow2((1.f - in_refractive_index) / (1.f + in_refractive_index));
    return r_0 + ((1.f - r_0) * glm::pow(1.f - in_cosine, 5.f));
}

static scatter_record scatter(const scene& in_scene, const dielectric_material& in_dielectric, const ray& in_ray, const hit_record& in_hit)
{
    const float direction_dot_normal = glm::dot(in_ray.direction, in_hit.normal);
    const float direction_length = glm::length(in_ray.direction);

    const auto [outward_normal, eta, cosine] = direction_dot_normal > 0.f
        ? std::make_tuple(-in_hit.normal, in_dielectric.refractive_index, in_dielectric.refractive_index * direction_dot_normal / direction_length)
        : std::make_tuple(in_hit.normal, 1.f / in_dielectric.refractive_index, -direction_dot_normal / direction_length);

    const displacement_3D refracted = glm::refract(glm::normalize(in_ray.direction), outward_normal, eta);

    const float reflect_probability = refracted != displacement_3D{ 0.f }
        ? Schlick(cosine, in_dielectric.refractive_index)
        : 1.f;

    const color albedo = color_on_texture(in_scene, in_dielectric.albedo, in_hit.mapping, in_hit.point);
    if (random_chance(reflect_probability))
    {
        const displacement_3D reflected = glm::reflect(in_ray.direction, in_hit.normal);
        return scatter_record{ albedo, ray{ line{ in_hit.point, reflected }, in_ray.time } };
    }
    return scatter_record{ albedo, ray{ line{ in_hit.point, refracted }, in_ray.time }, 1.f, false };
}

static scatter_record scatter(const scene& in_scene, const diffuse_material& in_diffuse, const ray& in_ray, const hit_record& in_hit)
{
    const direction_3D direction = ortho_normal_base{ in_hit.normal }.local(random_cosine_direction());
    const ray scattered_ray{ line{ in_hit.point, glm::normalize(direction) }, in_ray.time };
    const color albedo = color_on_texture(in_scene, in_diffuse.albedo, in_hit.mapping, in_hit.point);
    const float PDF = glm::dot(in_hit.normal, scattered_ray.direction) * glm::one_over_pi<float>();
    return scatter_record{ albedo, scattered_ray, PDF, false };
}

static scatter_record scatter(const scene& in_scene, const emit_light_material& in_emit_light, const ray& in_ray, const hit_record& in_hit)
{
    return scatter_record::nope();
}

static scatter_record scatter(const scene& in_scene, const reflect_material& in_reflect, const ray& in_ray, const hit_record& in_hit)
{
    const displacement_3D reflected = glm::reflect(in_ray.direction, in_hit.normal);
    const ray scattered = { line{ in_hit.point, reflected + (in_reflect.fuzz * random_in_unit_sphere()) }, in_ray.time };
    if (glm::dot(scattered.direction, in_hit.normal) > 0.f)
    {
        const color albedo = color_on_texture(in_scene, in_reflect.albedo, in_hit.mapping, in_hit.point);
        return scatter_record{ albedo, scattered, 1.f, true };
    }
    return scatter_record::nope();
}

scatter_record scatter(const scene& in_scene, const material& in_material, const ray& in_ray, const hit_record& in_hit)
{
    switch (in_material.type)
    {
        case material_type::dielectric: return scatter(in_scene, in_scene.dielectric_materials[in_material.index], in_ray, in_hit);
        case material_type::emit_light: return scatter(in_scene, in_scene.emit_light_materials[in_material.index], in_ray, in_hit);
        case material_type::diffuse:    return scatter(in_scene, in_scene.diffuse_materials[in_material.index], in_ray, in_hit);
        case material_type::reflect:    return scatter(in_scene, in_scene.reflect_materials[in_material.index], in_ray, in_hit);
        default: break;
    }
    return scatter_record::nope();
}