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
        position_3d{ -1.f,  0.5f, 0.5f }, }, -z_axis, {}, mirror_material);
    world.add_triangle_shape(triangle{
        position_3d{  1.f,  0.5f, 0.5f },
        position_3d{ -1.f,  0.5f, 0.5f },
        position_3d{  1.f, -0.2f, 0.5f }, }, -z_axis, {}, mirror_material);

    const material light_material = world.add_emit_light_material(world.add_constant_texture(white));
    world.add_triangle_shape(triangle{
        position_3d{ -1.f,  0.5f,  0.5f },
        position_3d{ -1.f, -0.2f,  0.5f },
        position_3d{ -1.f, -0.2f, -0.5f }, }, x_axis, {}, light_material);
    world.add_triangle_shape(triangle{
        position_3d{ -1.f,  0.5f,  0.5f },
        position_3d{ -1.f,  0.5f, -0.5f },
        position_3d{ -1.f, -0.2f, -0.5f }, }, x_axis, {}, light_material);

    world.add_sphere_shape(sphere{ position_3d{ -0.2f, 0.5f, 0.2f }, 0.2f }, y_axis,
        world.add_emit_light_material(world.add_constant_texture(color{ 1.f, 1.f, 0.5f })));

    return render_plan{ image_size, cam, std::move(world) };
}

render_plan render_plan::cornell_box(const extent_2d<uint32_t>& image_size)
{
    const camera cam = camera_create_info{
        position_3d{ 278.f, 278.f, -800.f },
        position_3d{ 278.f, 278.f, 0.f },
        y_axis,
        40.f,
        image_size.aspect(),
        0.f,
        { 0.f, 1.f }
    };

    scene world;
    world.sky = world.add_constant_texture(black);
    const material red_wall = world.add_diffuse_material(world.add_constant_texture(color{ 0.65f, 0.05f, 0.05f }));
    const material white_wall = world.add_diffuse_material(world.add_constant_texture(color{ 0.73f, 0.73f, 0.73f }));
    const material green_wall = world.add_diffuse_material(world.add_constant_texture(color{ 0.12f, 0.45f, 0.15f }));
    const material light = world.add_emit_light_material(world.add_constant_texture(color{ 15.f, 15.f, 15.f }));

    // left wall
    world.add_triangle_shape(triangle{
        position_3d{ 555.f,   0.f,   0.f },
        position_3d{ 555.f,   0.f, 555.f },
        position_3d{ 555.f, 555.f,   0.f }, }, -x_axis, {}, green_wall);
    world.add_triangle_shape(triangle{
        position_3d{ 555.f, 555.f, 555.f },
        position_3d{ 555.f,   0.f, 555.f },
        position_3d{ 555.f, 555.f,   0.f }, }, -x_axis, {}, green_wall);

    // right wall
    world.add_triangle_shape(triangle{
        position_3d{ 0.f,   0.f,   0.f },
        position_3d{ 0.f,   0.f, 555.f },
        position_3d{ 0.f, 555.f,   0.f }, }, x_axis, {}, red_wall);
    world.add_triangle_shape(triangle{
        position_3d{ 0.f, 555.f, 555.f },
        position_3d{ 0.f,   0.f, 555.f },
        position_3d{ 0.f, 555.f,   0.f }, }, x_axis, {}, red_wall);

    // light
    world.add_triangle_shape(triangle{
        position_3d{ 213.f, 554.f, 213.f },
        position_3d{ 213.f, 554.f, 343.f },
        position_3d{ 343.f, 554.f, 213.f }, }, -y_axis, {}, light);
    world.add_triangle_shape(triangle{
        position_3d{ 343.f, 554.f, 343.f },
        position_3d{ 213.f, 554.f, 343.f },
        position_3d{ 343.f, 554.f, 213.f }, }, -y_axis, {}, light);

    // ceiling
    world.add_triangle_shape(triangle{
        position_3d{   0.f, 555.f,   0.f },
        position_3d{   0.f, 555.f, 555.f },
        position_3d{ 555.f, 555.f,   0.f }, }, -y_axis, {}, white_wall);
    world.add_triangle_shape(triangle{
        position_3d{ 555.f, 555.f, 555.f },
        position_3d{   0.f, 555.f, 555.f },
        position_3d{ 555.f, 555.f,   0.f }, }, -y_axis, {}, white_wall);

    // floor
    world.add_triangle_shape(triangle{
        position_3d{   0.f, 0.f,   0.f },
        position_3d{   0.f, 0.f, 555.f },
        position_3d{ 555.f, 0.f,   0.f }, }, y_axis, {}, white_wall);
    world.add_triangle_shape(triangle{
        position_3d{ 555.f, 0.f, 555.f },
        position_3d{   0.f, 0.f, 555.f },
        position_3d{ 555.f, 0.f,   0.f }, }, y_axis, {}, white_wall);

    // back wall
    world.add_triangle_shape(triangle{
        position_3d{   0.f,   0.f, 555.f },
        position_3d{   0.f, 555.f, 555.f },
        position_3d{ 555.f,   0.f, 555.f }, }, -z_axis, {}, white_wall);
    world.add_triangle_shape(triangle{
        position_3d{ 555.f, 555.f, 555.f },
        position_3d{   0.f, 555.f, 555.f },
        position_3d{ 555.f,   0.f, 555.f }, }, -z_axis, {}, white_wall);

    return render_plan{ image_size, cam, std::move(world) };
}