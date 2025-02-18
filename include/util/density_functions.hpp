#pragma once

#include <render_objects/shapes.hpp>
#include <renderer_cpu/hit.hpp>
#include <renderer_cpu/ray.hpp>
#include <util/numeric.hpp>
#include <util/random.hpp>
#include <util/vector.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/optimum_pow.hpp>

// Cosine PDF

inline static float cosine_PDF_value(const ortho_normal_base& in_onb, const direction_3D& in_direction)
{
    const float cosine = glm::dot(in_direction, in_onb.w());
    return float(cosine >= 0) * cosine * glm::one_over_pi<float>();
}

inline static direction_3D cosine_PDF_generate(const ortho_normal_base& in_onb)
{
    return in_onb.local(random_cosine_direction());
}

// Plane PDF

inline static float plane_PDF_value(const plane_shape& in_plane, const ray& in_ray)
{
    if (const hit_record hit = ray_hits(in_plane, in_ray, { glm::epsilon<float>(), infinity<float> }); hit.occurred)
    {
        return 1.f;
    }
    return 0.f;
}

inline static displacement_3D plane_PDF_generate(const plane_shape& in_plane, const position_3D& in_position)
{
    return displacement_3D{ 0.f };
}

// Sphere PDF

inline static float sphere_PDF_value(const sphere_shape& in_sphere, const ray& in_ray)
{
    if (const hit_record hit = ray_hits(in_triangle, in_ray, { glm::epsilon<float>(), infinity<float> }); hit.occurred)
    {
        const float cos_theta_max = glm::sqrt(1.f - glm::pow2(in_sphere.radius) / square_length(in_sphere.origin - in_ray.origin));
        const float solid_angle = glm::two_pi<float>() * (1.f - cos_theta_max);
        return 1.f / solid_angle;
    }
    return 0.f;
}

inline static displacement_3D sphere_PDF_generate(const sphere_shape& in_sphere, const position_3D& in_position)
{
    const displacement_3D displacement = in_sphere.origin - in_position;
    return ortho_normal_base{ glm::normalize(displacement) }
        .local(random_to_sphere(in_sphere.radius, square_length(displacement)));
}

// Triangle PDF

inline static float triangle_PDF_value(const triangle_shape& in_triangle, const ray& in_ray)
{
    if (const hit_record hit = ray_hits(in_triangle, in_ray, { glm::epsilon<float>(), infinity<float> }); hit.occurred)
    {
        const float area = in_triangle.area();
        const float square_distance = glm::pow2(hit.distance) * square_length(in_ray.origin);
        const float cosine = glm::abs(glm::dot(in_ray.direction, hit.normal) * glm::length(in_ray.inverse_direction));
        return square_distance / (area * cosine);
    }
    return 0.f;
}

inline static displacement_3D triangle_PDF_generate(const triangle_shape& in_triangle, const position_3D& in_position)
{
    const float U = random_uniform(0.f, 1.f);
    const float V = random_uniform(0.f, U);
    return ((1.f - U - V) * in_triangle.a) + (U * in_triangle.b) + (V * in_triangle.c);
}