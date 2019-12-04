#include <render_objects/render_plan.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

render_plan render_plan::test_scene(const extent_2D<uint32_t>& image_size)
{
    const camera cam = camera_create_info{
        position_3D{ 0.75f, 0.35f, -1.25f },
        position_3D{ 0.f, 0.f, 0.f },
        y_axis,
        45.f,
        image_size.aspect(),
        0.025f,
        { 0.f, 1.f },
    };

    scene world;
    world.sky = world.add_image_texture(world.add_image("textures/sky_evening.jpg"),
        wrap_method::repeat, filtering_method::linear);

    { // floor
        const material grass = world.add_diffuse_material(
            world.add_image_texture(world.add_image("textures/grass.jpg"),
                wrap_method::repeat, filtering_method::catrom),
            world.add_normal_texture(world.add_normal_map("textures/grass_normal.jpg"),
                wrap_method::repeat, filtering_method::catrom));
        world.add_plane_shape(plane{ position_3D{ 0.f, -0.2f, 0.f }, x_axis, z_axis }, grass);
    }

    { // mirror-brick block
        const material mirror = world.add_reflect_material(0.01f,
            world.add_constant_texture(color{ 0.8f, 0.8f, 0.8f }),
            world.add_normal_texture(world.add_normal_map("textures/glass_normal.jpg"),
                wrap_method::clamp_to_edge, filtering_method::catrom));
        const material brick = world.add_diffuse_material(
            world.add_image_texture(world.add_image("textures/bricks.jpg"),
                wrap_method::clamp_to_edge, filtering_method::catrom),
            world.add_normal_texture(world.add_normal_map("textures/bricks_normal.jpg"),
                wrap_method::clamp_to_edge, filtering_method::catrom));
        world.assemble_cuboid({
            position_3D{ -0.35f, 0.1f, 1.f },
            extent_3D{ 0.85f, 0.3f, 0.5f },
            glm::quat{ glm::vec3{ 0.f, glm::radians(10.f), 0.f } },
            brick, brick, brick, brick, mirror, brick,
        });
    }

    { // lamp block
        const material lamp = world.add_emit_light_material(1.5f,
            world.add_image_texture(world.add_image("textures/redstone_lamp_on.png"),
                wrap_method::clamp_to_edge, filtering_method::nearest));
        world.assemble_cuboid({
            position_3D{ -0.2f, 0.5f, 0.2f },
            extent_3D{ 0.2f, 0.2f, 0.2f },
            glm::quat{ glm::vec3{ 0.f, glm::radians(25.f), 0.f } },
            lamp, lamp, lamp, lamp, lamp, lamp,
        });
    }

    { // grass block
        const uint32_t grass_image = world.add_image("textures/mc_grass.png");

        const material grass_bottom_face = world.add_diffuse_material(
            world.add_image_texture(grass_image, { { 0, 0 }, { 16, 16 } },
                wrap_method::clamp_to_edge, filtering_method::nearest));
        const material grass_top_face = world.add_diffuse_material(
            world.add_image_texture(grass_image, { { 0, 16 }, { 16, 32 } },
                wrap_method::clamp_to_edge, filtering_method::nearest));
        const material grass_side_face = world.add_diffuse_material(
            world.add_image_texture(grass_image, { { 16, 0 }, { 32, 16 } },
                wrap_method::clamp_to_edge, filtering_method::nearest));

        world.assemble_cuboid({
            position_3D{ 0.f, -0.1f, -0.3f },
            extent_3D{ 0.1f, 0.1f, 0.1f },
            glm::quat{ glm::vec3{ 0.f, glm::radians(45.f), 0.f } },
            grass_bottom_face, grass_top_face, grass_side_face, grass_side_face, grass_side_face, grass_side_face,
        });
    }

    // balls
    world.add_sphere_shape(sphere{ position_3D{ -0.6f, 0.f, 0.f }, 0.2f }, glm::normalize(displacement_3D{ 0.5f, 1.f, -0.5f }),
        world.add_diffuse_material(
            world.add_image_texture(world.add_image("textures/earth.jpg"),
                wrap_method::repeat, filtering_method::catrom),
            world.add_normal_texture(world.add_normal_map("textures/earth_normal.png"),
                wrap_method::repeat, filtering_method::catrom)));
    world.add_sphere_shape(sphere{ position_3D{ -0.2f, 0.f, 0.f }, 0.2f }, y_axis,
        world.add_reflect_material(0.015f,
            world.add_constant_texture(color{ 1.f, 0.7f, 0.8f })));
    world.add_sphere_shape(sphere{ position_3D{ 0.2f, 0.f, 0.f }, 0.2f }, y_axis,
        world.add_dielectric_material(1.5f,
            world.add_constant_texture(color{ 0.7f, 0.7f, 1.f })));

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
    const material no_wall = world.add_dielectric_material(1.f, world.add_constant_texture(color{ 1.f }));
    const material red_wall = world.add_diffuse_material(world.add_constant_texture(color{ 0.65f, 0.05f, 0.05f }));
    const material white_wall = world.add_diffuse_material(world.add_constant_texture(color{ 0.73f, 0.73f, 0.73f }));
    const material green_wall = world.add_diffuse_material(world.add_constant_texture(color{ 0.12f, 0.45f, 0.15f }));

    // box
    world.assemble_cuboid({
        position_3D{ 555.f * 0.5f },
        extent_3D{ 555.f * 0.5f, 555.f * 0.5f, 555.f * 0.5f },
        {},
        white_wall, white_wall, green_wall, red_wall, no_wall, white_wall,
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
        world.add_emit_light_material(15.f, world.add_constant_texture(white)),
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
    world.sky = world.add_image_texture(world.add_image("textures/sky.jpg"),
        wrap_method::repeat, filtering_method::linear);

    const uint32_t grass_image = world.add_image("textures/mc_grass.png");

    const material ground = world.add_diffuse_material(
        world.add_image_texture(grass_image, { { 0, 16 }, { 16, 32 } },
            wrap_method::repeat, filtering_method::nearest));

    world.add_plane_shape(plane{ position_3D{ -0.5f }, x_axis, z_axis }, ground);

    const material bottom_face = world.add_diffuse_material(
        world.add_image_texture(grass_image, { { 0, 0 }, { 16, 16 } },
            wrap_method::clamp_to_edge, filtering_method::nearest));
    const material top_face = world.add_diffuse_material(
        world.add_image_texture(grass_image, { { 0, 16 }, { 16, 32 } },
            wrap_method::clamp_to_edge, filtering_method::nearest));
    const material side_face = world.add_diffuse_material(
        world.add_image_texture(grass_image, { { 16, 0 }, { 32, 16 } },
            wrap_method::clamp_to_edge, filtering_method::nearest));

    world.assemble_cuboid({
        position_3D{ 0.f, 0.f, 0.f },
        extent_3D{ 0.5f, 0.5f, 0.5f },
        glm::quat{ glm::vec3{ 0.f, 0.f, 0.f } },
        bottom_face, top_face, side_face, side_face, side_face, side_face,
    });

    world.hierarchy = make_hierarchy(world.shapes);
    return render_plan{ image_size, cam, std::move(world) };
}

render_plan render_plan::bunny(const extent_2D<uint32_t>& image_size)
{
    const camera cam = camera_create_info{
        position_3D{ 2.5f, 2.f, 2.5f },
        position_3D{ 0.f, 0.75f, 0.f },
        y_axis,
        45.f,
        image_size.aspect(),
        0.05f,
        { 0.f, 1.f }
    };

    scene world;
    world.sky = world.add_image_texture(world.add_image("textures/sky.jpg"),
        wrap_method::repeat, filtering_method::linear);

    world.add_plane_shape(plane{ position_3D{ 0.f, 0.f, 0.f }, x_axis, z_axis },
        world.add_diffuse_material(world.add_constant_texture(color{ 0.4f, 0.8f, 0.3f })));

    { // bunny
        model_assembly_info bunny_info = load_model("models/bunny.obj");
//        bunny_info.mat = world.add_diffuse_material(world.add_constant_texture(white));
        bunny_info.mat = world.add_dielectric_material(1.77f, world.add_constant_texture(color{ 15.f, 82.f, 186.f } / 255.f));
//        bunny_info.mat = world.add_reflect_material(0.05f, world.add_constant_texture(white));
        world.assemble_model(bunny_info);
    }

    world.hierarchy = make_hierarchy(world.shapes);
    return render_plan{ image_size, cam, std::move(world) };
}