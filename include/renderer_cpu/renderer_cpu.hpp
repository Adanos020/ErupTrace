#pragma once

#include <util/colors.hpp>
#include <util/sizes.hpp>
#include <util/vector.hpp>

#include <future>
#include <vector>

class renderer_cpu
{
public:
    renderer_cpu(uint32_t sample_count, uint32_t thread_count);
    std::vector<rgba> render_scene(const struct render_plan&) const;
    color render_single_pixel(const struct render_plan&, const pixel_position&) const;

private:
    color render_pixel(const struct render_plan&, const pixel_position&, const extent_2D<float>& inverse_size) const;

private:
    const uint32_t sample_count;
    const uint32_t thread_count;

    const float inverse_sample_count;

    mutable std::mutex progress_mtx;
};