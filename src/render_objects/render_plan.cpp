#include <render_objects/render_plan.hpp>

render_plan render_plan::hello_ball(const extent_2d<uint32_t>& image_size)
{
    const camera cam = camera_create_info{
        position_3d{ 2.f, 0.3f, 2.f },
        position_3d{ 0.f, 0.f, 0.f },
        y_axis,
        45.f,
        image_size.aspect(),
        0.05f,
        { 0.f, 1.f }
    };

    scene world;
    world.sky = world.add_texture(constant_texture{ color{ 0.5f, 0.7f, 1.f } });
    world.add_shape(
        plane_shape{ plane{ position_3d{ 0.f, -0.2f, 0.f }, y_axis } },
        world.add_material(metal_material{ 0.025f,
            world.add_texture(constant_texture{ color{ 0.2f, 1.f, 0.f } }) }));
    world.add_shape(
        sphere_shape{ sphere{ position_3d{ -0.2f, 0.f, 0.f }, 0.2f } },
        world.add_material(lambertian_material{
            world.add_texture(checker_texture{ transform_2d{}.scale_to(30.f),
                color{ 0.5f, 0.7f, 1.f }, color{ 1.f, 0.3f, 0.5f } }) }));
    world.add_shape(
        sphere_shape{ sphere{ position_3d{ 0.2f, 0.f, 0.f }, 0.2f } },
        world.add_material(dielectric_material{ 1.5f,
            world.add_texture(constant_texture{ color{ 0.7f, 0.7f, 1.f } }) }));

    return render_plan{ image_size, cam, std::move(world) };
}