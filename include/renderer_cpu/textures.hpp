#pragma once

#include <util/colors.hpp>
#include <util/vector_types.hpp>

color color_on_texture(const struct scene&, const struct texture&, const struct uv_mapping&, const position_3d&);