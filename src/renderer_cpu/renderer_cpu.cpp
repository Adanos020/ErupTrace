#include <renderer_cpu/renderer_cpu.hpp>

#include <render_objects/render_plan.hpp>
#include <renderer_cpu/ray.hpp>
#include <util/random.hpp>

#include <iomanip>
#include <iostream>

renderer_cpu::renderer_cpu(const uint32_t sample_count, const uint32_t thread_count)
    : sample_count(sample_count)
    , thread_count(thread_count)
    , inverse_sample_count(1.f / sample_count)
{
}

std::vector<rgba> renderer_cpu::render_scene(const render_plan& plan)
{
    const uint32_t fragment_height = plan.image_size.height / this->thread_count;

    std::cout << "Starting jobs... ";

    std::vector<std::future<std::vector<rgba>>> image_fragments{ this->thread_count };
    for (uint32_t i = 0; i < image_fragments.size(); ++i)
    {
        image_fragments[i] = std::async(std::launch::async, &renderer_cpu::render_fragment, this,
            &plan, glm::uvec2{ 0, i * fragment_height },
            glm::uvec2{ plan.image_size.width, (i + 1) * fragment_height });
    }

    std::cout << "Done." << std::endl;
    std::cout << "Rendering image fragments... 0.00%";

    this->progress = 0.f;
    std::vector<rgba> image;
    for (uint32_t i = 0; i < image_fragments.size(); ++i)
    {
        const std::vector<rgba> fragment = image_fragments[i].get();
        image.insert(image.end(), fragment.begin(), fragment.end());
    }

    std::cout << "\rRendering image fragments... Done.  " << std::endl;
    return image;
}

color renderer_cpu::render_single_pixel(const render_plan& in_plan, const pixel_position& in_position)
{
    return this->render_pixel(in_plan, in_position, { 1.f / in_plan.image_size.width, 1.f / in_plan.image_size.height });
}

std::vector<rgba> renderer_cpu::render_fragment(const render_plan* in_plan, const position_2D& top_left, const position_2D& bottom_right)
{
    const uint32_t width = bottom_right.x - top_left.x;
    const uint32_t height = bottom_right.y - top_left.y;
    const extent_2D<float> inverse_image_size = {
        1.f / in_plan->image_size.width,
        1.f / in_plan->image_size.height,
    };

    std::vector<rgba> image_fragment;
    image_fragment.reserve(width * height);

    for (uint32_t y = top_left.y; y < bottom_right.y; ++y)
    {
        for (uint32_t x = top_left.x; x < bottom_right.x; ++x)
        {
            const color col = render_pixel(*in_plan, { x, y }, inverse_image_size);
            image_fragment.push_back(rgba{ to_rgb(col), 255 });
        }

        std::lock_guard lock{ this->progress_mtx };
        this->progress += 100.f * inverse_image_size.height;
        std::cout << "\rRendering image fragments... " << std::fixed << std::setprecision(2) << this->progress << "%";
    }
    return image_fragment;
}

color renderer_cpu::render_pixel(const render_plan& in_plan, const pixel_position& in_position, const extent_2D<float>& in_inverse_size)
{
    color col{ 0.f };
    for (uint32_t s = 0; s < sample_count; ++s)
    {
        const barycentric_2D ray_direction = {
            (in_position.x + random_uniform<float>()) * in_inverse_size.width,
            (in_plan.image_size.height - in_position.y + random_uniform<float>()) * in_inverse_size.height,
        };
        col += ray::shoot(in_plan.cam, ray_direction).trace(in_plan.world);
    }
    col *= this->inverse_sample_count;
    return col = { glm::sqrt(col.r), glm::sqrt(col.g), glm::sqrt(col.b) };
}