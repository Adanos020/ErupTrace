#pragma once

#include <util/colors.hpp>
#include <renderer_cpu/hit.hpp>
#include <render_objects/materials.hpp>

color emit(const scene& in_scene, const material& in_emit_light, const hit_record& in_hit);