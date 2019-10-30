#include <renderer_cpu/ray.hpp>

#include <render_objects/camera.hpp>
#include <render_objects/textures.hpp>
#include <render_objects/scene.hpp>
#include <renderer_cpu/emitting.hpp>
#include <renderer_cpu/hit.hpp>
#include <renderer_cpu/scattering.hpp>
#include <renderer_cpu/textures.hpp>
#include <util/random.hpp>

// Rays

ray::ray()
    : line(line{ position_3d{ 0.f }, direction_3d{ 0.f } })
    , inverse_direction(0.f)
    , time(0.f)
{
}

ray::ray(const line& l, const float time)
    : line(l)
    , inverse_direction(1.f / l.direction)
    , time(time)
{
}

ray ray::shoot(const camera& in_camera, const uv_mapping& in_direction)
{
    const displacement_3d random_spot_on_lens = in_camera.lens_radius * random_in_unit_disk();
    const displacement_3d offset = (in_camera.u * random_spot_on_lens.x) + (in_camera.v * random_spot_on_lens.y);
    const position_3d origin = in_camera.origin + offset;

    return ray{
        line { origin,
            in_camera.lower_left_corner + (in_direction.u * in_camera.horizontal) + (in_direction.v * in_camera.vertical) - origin,
        },
        random_uniform(in_camera.time.min, in_camera.time.max),
    };
}

color ray::trace(const scene& in_scene, const int32_t depth) const
{
    if (depth > 0)
    {
        hit_record closest_hit = hit_record::nope();
        closest_hit.distance = FLT_MAX;
        for (const shape& it_shape : in_scene.shapes)
        {
            const hit_record hit = ray_hits(in_scene, it_shape, *this, min_max<float>{ 0.0001f, closest_hit.distance });
            if (hit.occurred)
            {
                closest_hit = hit;
            }
        }

        if (closest_hit.occurred)
        {
            const color emitted = emit(in_scene, closest_hit.mat, closest_hit);
            if (const scattering sc = scatter(in_scene, closest_hit.mat, *this, closest_hit); sc.occurred)
            {
                return emitted + sc.attenuation * sc.scattered_ray.trace(in_scene, depth - 1);
            }
            return emitted;
        }
    }
    return color_on_texture(in_scene, in_scene.sky, uv_on_sphere(glm::normalize(this->direction), y_axis),
        this->origin + this->direction);
}