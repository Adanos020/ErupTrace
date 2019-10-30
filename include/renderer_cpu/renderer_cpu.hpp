#pragma once

#include <util/colors.hpp>
#include <util/vector_types.hpp>

#include <future>
#include <vector>

class renderer_cpu
{
public:
    renderer_cpu(uint32_t sample_count, uint32_t thread_count);
    std::vector<rgba> render_scene(const struct render_plan&);

private:
    std::vector<rgba> render_fragment(const struct render_plan*, const position_2d& top_left, const position_2d& bottom_right);

private:
    const uint32_t sample_count;
    const uint32_t thread_count;

    float progress = 0.f;
    mutable std::mutex progress_mtx;
};