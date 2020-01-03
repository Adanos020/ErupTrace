#include <renderer_cpu/ray.hpp>

#include <render_objects/camera.hpp>
#include <render_objects/textures.hpp>
#include <render_objects/scene.hpp>
#include <renderer_cpu/emitting.hpp>
#include <renderer_cpu/hit.hpp>
#include <renderer_cpu/scattering.hpp>
#include <renderer_cpu/textures.hpp>
#include <util/density_functions.hpp>
#include <util/random.hpp>

#define DRAW_NORMALS 0

// Rays

ray::ray()
    : line(line{ position_3D{ 0.f }, direction_3D{ 0.f } })
    , inverse_direction(0.f)
    , time(0.f)
{
}

ray::ray(const line& in_line, const float in_time)
    : line(in_line)
    , inverse_direction(1.f / in_line.direction)
    , time(in_time)
{
}

ray ray::shoot(const camera& in_camera, const barycentric_2D& in_screen_UV)
{
    const displacement_3D random_spot_on_lens = in_camera.lens_radius * random_in_unit_disk();
    const displacement_3D offset = (in_camera.u * random_spot_on_lens.x) + (in_camera.v * random_spot_on_lens.y);
    const position_3D origin = in_camera.origin + offset;

    return ray{
        line { origin,
            glm::normalize(in_camera.lower_left_corner + (in_screen_UV.U * in_camera.horizontal)
                + (in_screen_UV.V * in_camera.vertical) - origin),
        },
        random_uniform(in_camera.time.min, in_camera.time.max),
    };
}

color ray::trace(const scene& in_scene, const int32_t depth) const
{
    if (depth > 0)
    {
        if (const hit_record hit = ray_hits_anything(in_scene, *this); hit.occurred)
        {
#if DRAW_NORMALS
            return color{ 0.5f } + color{ 0.5f * hit.normal };
#else
            const color emitted = emit(in_scene, hit.mat, hit);
            if (scatter_record scattering = scatter(in_scene, hit.mat, *this, hit); scattering.occurred)
            {
                if (scattering.is_specular)
                {
                    return scattering.albedo * scattering.scattered_ray.trace(in_scene, depth - 1);
                }
                else
                {
                    const array_index resampled_index = random_uniform<size_t>(0, in_scene.resampled_shapes.size() - 1);
                    const shape& resampled = in_scene.resampled_shapes[resampled_index];

                    const float resampled_PDF = shape_PDF_value(in_scene, resampled, scattering.scattered_ray);
                    const float PDF = glm::mix(resampled_PDF, scattering.PDF, 0.5f);

                    if (random_chance())
                    {
                        scattering.scattered_ray.direction = shape_PDF_generate(in_scene, resampled, hit.point);
                    }
                    
                    const color next_rays_color = scattering.scattered_ray.trace(in_scene, depth - 1);
                    return emitted + (scattering.albedo * scattering.PDF * next_rays_color / PDF);
                }
            }
            return emitted;
#endif
        }
    }
    const direction_3D unit_direction = glm::normalize(this->direction);
    return color_on_texture(in_scene, in_scene.sky, mapping_on_sphere(unit_direction, y_axis), this->origin + this->direction);
}