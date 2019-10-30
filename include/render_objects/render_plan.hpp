#pragma once

#include <render_objects/camera.hpp>
#include <render_objects/scene.hpp>
#include <util/sizes.hpp>

struct render_plan
{
    extent_2d<uint32_t> image_size;
    camera cam;
    scene world;

    static render_plan test_scene(const extent_2d<uint32_t>& image_size);
};