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
    std::vector<rgba> render_scene(const struct render_plan&);
    color render_single_pixel(const struct render_plan&, const pixel_position&);

private:
    std::vector<rgba> render_fragment(const struct render_plan*, const position_2D& top_left, const position_2D& bottom_right);
    color render_pixel(const struct render_plan&, const pixel_position&, const extent_2D<float>& inverse_size);

private:
    const uint32_t sample_count;
    const uint32_t thread_count;

    const float inverse_sample_count;

    float progress = 0.f;
    mutable std::mutex progress_mtx;
};