#pragma once

#include <util/colors.hpp>
#include <util/pairs.hpp>
#include <util/vector.hpp>

color color_on_texture(const struct scene&, const struct texture&, const struct barycentric_2D&, const position_3D&);
direction_3D normal_on_texture(const struct scene&, const struct normal_texture&, const struct barycentric_2D&);