#pragma once

#include <util/colors.hpp>

#include <glm/glm.hpp>

#include <future>
#include <vector>

class renderer_cpu
{
public:
    renderer_cpu(const uint32_t sample_count, const uint32_t thread_count);
    std::vector<rgba> render_scene(const struct render_plan&);

private:
    std::vector<rgba> render_fragment(const struct render_plan*, const glm::uvec2 top_left, const glm::uvec2 bottom_right);

private:
    const uint32_t sample_count;
    const uint32_t thread_count;

    float progress = 0.f;
    mutable std::mutex progress_mtx;
};