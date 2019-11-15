#include <render_objects/render_plan.hpp>

render_plan render_plan::test_scene(const extent_2D<uint32_t>& image_size)
{
    const camera cam = camera_create_info{
        position_3D{ 0.5f, 0.35f, -1.25f },
        position_3D{ 0.f, 0.f, 0.f },
        y_axis,
        45.f,
        image_size.aspect(),
        0.05f,
        { 0.f, 1.f }
    };

    scene world;
    world.sky = world.add_image_texture(
        world.add_image("textures/sky.jpg"), { { 0, 0 }, { 2880, 1440 } },
        image::wrap_method::repeat, image::filtering_method::linear);

    // floor
    world.add_plane_shape(plane{ position_3D{ 0.f, -0.2f, 0.f }, y_axis }, {},
        world.add_diffuse_material(
            world.add_constant_texture({ color{ 0.7f, 1.f, 0.3f } })));

    // walls
    const material mirror_material = world.add_reflect_material(0.01f, world.add_constant_texture(color{ 0.8f, 0.8f, 0.8f }));
    world.add_triangle_shape(triangle{
        position_3D{ -1.f, -0.2f, 0.5f },
        position_3D{  1.f, -0.2f, 0.5f },
        position_3D{ -1.f,  0.5f, 0.5f }, }, -z_axis, {}, mirror_material);
    world.add_triangle_shape(triangle{
        position_3D{  1.f,  0.5f, 0.5f },
        position_3D{ -1.f,  0.5f, 0.5f },
        position_3D{  1.f, -0.2f, 0.5f }, }, -z_axis, {}, mirror_material);

    const material light_material = world.add_emit_light_material(world.add_constant_texture(white), 1.f);
    world.add_triangle_shape(triangle{
        position_3D{ -1.f,  0.5f,  0.5f },
        position_3D{ -1.f, -0.2f,  0.5f },
        position_3D{ -1.f, -0.2f, -0.5f }, }, x_axis, {}, light_material);
    world.add_triangle_shape(triangle{
        position_3D{ -1.f,  0.5f,  0.5f },
        position_3D{ -1.f,  0.5f, -0.5f },
        position_3D{ -1.f, -0.2f, -0.5f }, }, x_axis, {}, light_material);

    // balls
    world.add_sphere_shape(sphere{ position_3D{ -0.2f, 0.f, 0.f }, 0.2f }, glm::normalize(displacement_3D{ 0.5f, 1.f, -0.5f }),
        world.add_diffuse_material(
            world.add_image_texture(
                world.add_image("textures/earth.jpg"), { { 0, 0 }, { 8192, 4096 } },
                image::wrap_method::repeat, image::filtering_method::linear)));
    world.add_sphere_shape(sphere{ position_3D{ 0.2f, 0.f, 0.f }, 0.2f }, y_axis,
        world.add_dielectric_material(1.5f,
            world.add_constant_texture(color{ 0.7f, 0.7f, 1.f })));

    world.add_sphere_shape(sphere{ position_3D{ -0.2f, 0.5f, 0.2f }, 0.2f }, y_axis,
        world.add_emit_light_material(
            world.add_constant_texture(color{ 1.f, 1.f, 0.5f }), 1.5f));

    world.hierarchy = make_hierarchy(world.shapes);
    return render_plan{ image_size, cam, std::move(world) };
}

render_plan render_plan::cornell_box(const extent_2D<uint32_t>& image_size)
{
    const camera cam = camera_create_info{
        position_3D{ 278.f, 278.f, -800.f },
        position_3D{ 278.f, 278.f, 0.f },
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
    const material light = world.add_emit_light_material(world.add_constant_texture(white), 15.f);

    // left wall
    world.add_triangle_shape(triangle{
        position_3D{ 555.f,   0.f,   0.f },
        position_3D{ 555.f,   0.f, 555.f },
        position_3D{ 555.f, 555.f,   0.f }, }, -x_axis, {}, green_wall);
    world.add_triangle_shape(triangle{
        position_3D{ 555.f, 555.f, 555.f },
        position_3D{ 555.f,   0.f, 555.f },
        position_3D{ 555.f, 555.f,   0.f }, }, -x_axis, {}, green_wall);

    // right wall
    world.add_triangle_shape(triangle{
        position_3D{ 0.f,   0.f,   0.f },
        position_3D{ 0.f,   0.f, 555.f },
        position_3D{ 0.f, 555.f,   0.f }, }, x_axis, {}, red_wall);
    world.add_triangle_shape(triangle{
        position_3D{ 0.f, 555.f, 555.f },
        position_3D{ 0.f,   0.f, 555.f },
        position_3D{ 0.f, 555.f,   0.f }, }, x_axis, {}, red_wall);

    // light
    world.add_triangle_shape(triangle{
        position_3D{ 213.f, 554.f, 213.f },
        position_3D{ 213.f, 554.f, 343.f },
        position_3D{ 343.f, 554.f, 213.f }, }, -y_axis, {}, light);
    world.add_triangle_shape(triangle{
        position_3D{ 343.f, 554.f, 343.f },
        position_3D{ 213.f, 554.f, 343.f },
        position_3D{ 343.f, 554.f, 213.f }, }, -y_axis, {}, light);

    // ceiling
    world.add_triangle_shape(triangle{
        position_3D{   0.f, 555.f,   0.f },
        position_3D{   0.f, 555.f, 555.f },
        position_3D{ 555.f, 555.f,   0.f }, }, -y_axis, {}, white_wall);
    world.add_triangle_shape(triangle{
        position_3D{ 555.f, 555.f, 555.f },
        position_3D{   0.f, 555.f, 555.f },
        position_3D{ 555.f, 555.f,   0.f }, }, -y_axis, {}, white_wall);

    // floor
    world.add_triangle_shape(triangle{
        position_3D{   0.f, 0.f,   0.f },
        position_3D{   0.f, 0.f, 555.f },
        position_3D{ 555.f, 0.f,   0.f }, }, y_axis, {}, white_wall);
    world.add_triangle_shape(triangle{
        position_3D{ 555.f, 0.f, 555.f },
        position_3D{   0.f, 0.f, 555.f },
        position_3D{ 555.f, 0.f,   0.f }, }, y_axis, {}, white_wall);

    // back wall
    world.add_triangle_shape(triangle{
        position_3D{   0.f,   0.f, 555.f },
        position_3D{   0.f, 555.f, 555.f },
        position_3D{ 555.f,   0.f, 555.f }, }, -z_axis, {}, white_wall);
    world.add_triangle_shape(triangle{
        position_3D{ 555.f, 555.f, 555.f },
        position_3D{   0.f, 555.f, 555.f },
        position_3D{ 555.f,   0.f, 555.f }, }, -z_axis, {}, white_wall);

    world.hierarchy = make_hierarchy(world.shapes);
    return render_plan{ image_size, cam, std::move(world) };
}