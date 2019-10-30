#include <render_objects/render_plan.hpp>

render_plan render_plan::test_scene(const extent_2d<uint32_t>& image_size)
{
    const camera cam = camera_create_info{
        position_3d{ 0.5f, 0.35f, -1.25f },
        position_3d{ 0.f, 0.f, 0.f },
        y_axis,
        45.f,
        image_size.aspect(),
        0.05f,
        { 0.f, 1.f }
    };

    scene world;
    world.sky = world.add_constant_texture(color{ 0.125f, 0.175f, 0.25f });

    world.add_plane_shape(plane{ position_3d{ 0.f, -0.2f, 0.f }, y_axis }, {},
        world.add_diffuse_material(
            world.add_constant_texture({ color{ 0.7f, 1.f, 0.3f } })));

    world.add_sphere_shape(sphere{ position_3d{ -0.2f, 0.f, 0.f }, 0.2f }, glm::normalize(displacement_3d{ 1.f, 1.f, -1.f }),
        world.add_diffuse_material(
            world.add_checker_texture(transform_2d{}.scale_to(30.f), color{ 0.5f, 0.7f, 1.f }, color{ 1.f, 0.3f, 0.5f })));
    world.add_sphere_shape(sphere{ position_3d{ 0.2f, 0.f, 0.f }, 0.2f }, y_axis,
        world.add_dielectric_material(1.5f,
            world.add_constant_texture(color{ 0.7f, 0.7f, 1.f })));

    const material mirror_material = world.add_reflect_material(0.01f, world.add_constant_texture(color{ 0.8f, 0.8f, 0.8f }));
    world.add_triangle_shape(triangle{
        position_3d{ -1.f, -0.2f, 0.5f },
        position_3d{  1.f, -0.2f, 0.5f },
        position_3d{ -1.f,  0.5f, 0.5f }, }, direction_3d{ 0.f, 0.f, -1.f }, {}, mirror_material);
    world.add_triangle_shape(triangle{
        position_3d{  1.f,  0.5f, 0.5f },
        position_3d{ -1.f,  0.5f, 0.5f },
        position_3d{  1.f, -0.2f, 0.5f }, }, direction_3d{ 0.f, 0.f, -1.f }, {}, mirror_material);

    const material light_material = world.add_emit_light_material(world.add_constant_texture(white));
    world.add_triangle_shape(triangle{
        position_3d{ -1.f,  0.5f,  0.5f },
        position_3d{ -1.f, -0.2f,  0.5f },
        position_3d{ -1.f, -0.2f, -0.5f }, }, direction_3d{ 0.f, 0.f, -1.f }, {}, light_material);
    world.add_triangle_shape(triangle{
        position_3d{ -1.f,  0.5f,  0.5f },
        position_3d{ -1.f,  0.5f, -0.5f },
        position_3d{ -1.f, -0.2f, -0.5f }, }, direction_3d{ 0.f, 0.f, -1.f }, {}, light_material);

    world.add_sphere_shape(sphere{ position_3d{ -0.2f, 0.5f, 0.2f }, 0.2f }, y_axis,
        world.add_emit_light_material(world.add_constant_texture(color{ 1.f, 1.f, 0.5f })));

    return render_plan{ image_size, cam, std::move(world) };
}