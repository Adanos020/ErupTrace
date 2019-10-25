#pragma once

#include <util/pairs.hpp>
#include <util/vector_types.hpp>

struct camera_create_info
{
    position_3d camera_position;
    position_3d looking_at;
    direction_3d up;
    float vertical_fov;
    float aspect_ratio;
    float aperture;
    min_max<float> time;
};

struct camera
{
    position_3d origin;
    position_3d lower_left_corner;
    position_3d horizontal;
    position_3d vertical;
    displacement_3d w;
    displacement_3d u;
    displacement_3d v;
    float lens_radius;
    min_max<float> time;

    camera(const camera_create_info&);
};