#include <renderer_cpu/renderer_cpu.hpp>

#include <render_objects/hierarchy.hpp>
#include <render_objects/render_plan.hpp>
#include <renderer_cpu/ray.hpp>
#include <util/random.hpp>

#include <iomanip>
#include <iostream>

renderer_cpu::renderer_cpu(const uint32_t sample_count, const uint32_t thread_count)
    : sample_count(sample_count)
    , thread_count(thread_count)
{
}

std::vector<rgba> renderer_cpu::render_scene(const render_plan& plan)
{
    bounding_interval_hierarchy hierarchy = make_hierarchy(plan.world.shapes, 1.f / 6.f);

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

std::vector<rgba> renderer_cpu::render_fragment(const render_plan* plan, const position_2d& top_left, const position_2d& bottom_right)
{
    const uint32_t width = bottom_right.x - top_left.x;
    const uint32_t height = bottom_right.y - top_left.y;
    const float inverse_image_width = 1.f / plan->image_size.width;
    const float inverse_image_height = 1.f / plan->image_size.height;
    const float inverse_sample_count = 1.f / sample_count;

    std::vector<rgba> image_fragment;
    image_fragment.reserve(size_t(width) * size_t(height));

    for (uint32_t y = top_left.y; y < bottom_right.y; ++y)
    {
        for (uint32_t x = top_left.x; x < bottom_right.x; ++x)
        {
            color col{ 0.f };
            for (uint32_t s = 0; s < sample_count; ++s)
            {
                const uv_mapping ray_direction = {
                    float(x + random_uniform<float>()) * inverse_image_width,
                    float(plan->image_size.height - y + random_uniform<float>())* inverse_image_height,
                };
                const ray r = ray::shoot(plan->cam, ray_direction);
                col += r.trace(plan->world);
            }
            col *= inverse_sample_count;
            col = { glm::sqrt(col.r), glm::sqrt(col.g), glm::sqrt(col.b) };

            image_fragment.push_back(rgba{ to_rgb(col), 255 });
        }

        std::lock_guard lock{ this->progress_mtx };
        this->progress += 100.f * inverse_image_height;
        std::cout << "\rRendering image fragments... " << std::fixed << std::setprecision(2) << this->progress << "%";
    }
    return image_fragment;
}