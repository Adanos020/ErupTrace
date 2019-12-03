#pragma once

#include <render_objects/materials.hpp>
#include <util/sizes.hpp>
#include <util/vector.hpp>
#include <util/vertex.hpp>

#include <external/tiny_obj_loader.h>

#include <glm/gtx/quaternion.hpp>

#include <array>
#include <vector>

struct quad_assembly_info
{
    std::array<position_3D, 4> vertices;
    std::array<direction_3D, 4> normals;
    std::array<barycentric_2D, 4> mappings;
    material mat;
};

struct cuboid_assembly_info
{
    position_3D origin;
    extent_3D<float> half_size;
    glm::quat rotation;
    material bottom_face;
    material top_face;
    material left_face;
    material right_face;
    material front_face;
    material back_face;
    bool face_inwards = false;
};

struct model_assembly_info
{
    std::vector<vertex> vertices;
    material mat;
};

model_assembly_info load_model(const std::string_view path);