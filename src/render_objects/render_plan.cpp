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
    world.sky = world.add_image_texture(world.add_image("textures/sky_evening.jpg"),
        image::wrap_method::repeat, image::filtering_method::linear);

    // floor
    world.add_plane_shape(plane{ position_3D{ 0.f, -0.2f, 0.f }, y_axis }, {},
        world.add_diffuse_material(
            world.add_constant_texture({ color{ 0.7f, 1.f, 0.3f } })));

    // walls
    world.assemble_quad({
        {
            position_3D{ -1.2f, -0.2f, +0.5f },
            position_3D{ +0.5f, -0.2f, +0.5f },
            position_3D{ +0.5f, +0.5f, +0.5f },
            position_3D{ -1.2f, +0.5f, +0.5f },
        },
        { -z_axis, -z_axis, -z_axis, -z_axis },
        {},
        world.add_reflect_material(0.01f, world.add_constant_texture(color{ 0.8f, 0.8f, 0.8f })),
    });

    // balls
    world.add_sphere_shape(sphere{ position_3D{ -0.6f, 0.f, 0.f }, 0.2f }, glm::normalize(displacement_3D{ 0.5f, 1.f, -0.5f }),
        world.add_diffuse_material(world.add_image_texture(
            world.add_image("textures/earth.jpg"), { { 0, 0 }, { 8192, 4096 } },
            image::wrap_method::repeat, image::filtering_method::linear)));
    world.add_sphere_shape(sphere{ position_3D{ -0.2f, 0.f, 0.f }, 0.2f }, y_axis,
        world.add_reflect_material(0.015f,
            world.add_constant_texture(color{ 1.f, 0.7f, 0.8f })));
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
    const material no_wall = world.add_dielectric_material(0.f, world.add_constant_texture(color{ 1.f }));
    const material red_wall = world.add_diffuse_material(world.add_constant_texture(color{ 0.65f, 0.05f, 0.05f }));
    const material white_wall = world.add_diffuse_material(world.add_constant_texture(color{ 0.73f, 0.73f, 0.73f }));
    const material green_wall = world.add_diffuse_material(world.add_constant_texture(color{ 0.12f, 0.45f, 0.15f }));
    const material light = world.add_emit_light_material(world.add_constant_texture(white), 15.f);

    // box
    world.assemble_cuboid({
        position_3D{ 555.f * 0.5f },
        extent_3D{ 555.f * 0.5f, 555.f * 0.5f, 555.f * 0.5f },
        {},
        white_wall,
        white_wall,
        green_wall,
        red_wall,
        no_wall,
        white_wall,
        true,
    });

    // light
    world.assemble_quad({
        {
            position_3D{ 213.f, 554.f, 213.f },
            position_3D{ 343.f, 554.f, 213.f },
            position_3D{ 343.f, 554.f, 343.f },
            position_3D{ 213.f, 554.f, 343.f },
        },
        { -y_axis, -y_axis, -y_axis, -y_axis },
        {},
        light,
    });

    world.hierarchy = make_hierarchy(world.shapes);
    return render_plan{ image_size, cam, std::move(world) };
}

render_plan render_plan::grass_block(const extent_2D<uint32_t>& image_size)
{
    const camera cam = camera_create_info{
        position_3D{ 2.f, 0.75f, -2.5f },
        position_3D{ 0.f, 0.f, 0.f },
        y_axis,
        45.f,
        image_size.aspect(),
        0.05f,
        { 0.f, 1.f }
    };

    scene world;
    world.sky = world.add_image_texture(
        world.add_image("textures/stars_milky_way.jpg"), { { 0, 0 }, { 2880, 1440 } },
        image::wrap_method::repeat, image::filtering_method::linear);

    const uint32_t grass_image = world.add_image("textures/mc_grass.png");

    const material bottom_face = world.add_diffuse_material(
        world.add_image_texture(grass_image, { { 0, 0 }, { 16, 16 } },
            image::wrap_method::repeat, image::filtering_method::nearest));
    const material top_face = world.add_diffuse_material(
        world.add_image_texture(grass_image, { { 0, 16 }, { 16, 32 } },
            image::wrap_method::repeat, image::filtering_method::nearest));
    const material side_face = world.add_diffuse_material(
        world.add_image_texture(grass_image, { { 16, 0 }, { 32, 16 } },
            image::wrap_method::repeat, image::filtering_method::nearest));

    world.add_plane_shape(plane{ position_3D{ 0.f, -0.5f, 0.f }, y_axis }, {},
        world.add_diffuse_material(
            world.add_constant_texture({ color{ 0.7f, 1.f, 0.3f } })));

    world.assemble_cuboid({
        position_3D{ 0.f, 0.f, 0.f },
        extent_3D{ 0.5f, 0.5f, 0.5f },
        {},
        bottom_face,
        top_face,
        side_face,
        side_face,
        side_face,
        side_face,
    });
 
    world.add_sphere_shape(sphere{ position_3D{ 0.5f, 2.f, 0.5f }, 0.7f }, y_axis,
        world.add_emit_light_material(
            world.add_constant_texture(color{ 1.f, 1.f, 0.5f }), 1.5f));

    world.hierarchy = make_hierarchy(world.shapes);
    return render_plan{ image_size, cam, std::move(world) };
}