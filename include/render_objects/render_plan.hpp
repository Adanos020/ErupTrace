#pragma once

#include <render_objects/camera.hpp>
#include <render_objects/scene.hpp>
#include <util/sizes.hpp>

struct render_plan
{
    extent_2D<uint32_t> image_size;
    camera cam;
    scene world;

    static render_plan test_scene(const extent_2D<uint32_t>& image_size);
    static render_plan cornell_box(const extent_2D<uint32_t>& image_size);
    static render_plan grass_block(const extent_2D<uint32_t>& image_size);
    static render_plan bunny(const extent_2D<uint32_t>& image_size);
};