#pragma once

#include <util/colors.hpp>
#include <renderer_cpu/hit.hpp>
#include <render_objects/materials.hpp>

color emit(const scene& in_scene, const material& in_material, const hit_record& in_hit);