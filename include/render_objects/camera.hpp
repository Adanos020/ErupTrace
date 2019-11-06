#pragma once

#include <util/pairs.hpp>
#include <util/vector_types.hpp>

struct camera_create_info
{
    position_3D camera_position;
    position_3D looking_at;
    direction_3D up;
    float vertical_fov;
    float aspect_ratio;
    float aperture;
    min_max<float> time;
};

struct camera
{
    position_3D origin;
    position_3D lower_left_corner;
    position_3D horizontal;
    position_3D vertical;
    displacement_3D w;
    displacement_3D u;
    displacement_3D v;
    float lens_radius;
    min_max<float> time;

    camera(const camera_create_info&);
};