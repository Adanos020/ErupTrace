#include <renderer_cpu/renderer_cpu.hpp>

#include <render_objects/render_plan.hpp>
#include <renderer_cpu/ray.hpp>
#include <util/random.hpp>

#include <iomanip>
#include <iostream>

renderer_cpu::renderer_cpu(const uint32_t sample_count, const uint32_t thread_count)
    : sample_count(sample_count)
    , thread_count(glm::clamp<uint32_t>(thread_count, 1, std::thread::hardware_concurrency()))
    , inverse_sample_count(1.f / sample_count)
{
    std::cout << "Rendering on " << this->thread_count << " CPU threads." << std::endl;
}

std::vector<rgba> renderer_cpu::render_scene(const render_plan& in_plan) const
{
    std::cout << "Rendering image fragments... 0.00%";

    const extent_2D<float> inverse_image_size = {
        1.f / in_plan.image_size.width,
        1.f / in_plan.image_size.height,
    };
    const size_t pixel_count = in_plan.image_size.width * in_plan.image_size.height;
    const float inverse_pixel_count = 1.f / float(pixel_count);

    std::vector<rgba> pixels(pixel_count);
    {
        std::vector<std::future<void>> jobs;
        jobs.reserve(this->thread_count);

        this->progress = 0.f;
        for (size_t i = 0; i < this->thread_count; ++i)
        {
            jobs.emplace_back(std::async(std::launch::async, [=, &in_plan, &pixels, &inverse_image_size]() {
                for (size_t p = i; p < pixel_count; p += this->thread_count)
                {
                    const pixel_position pixel = {
                        p % in_plan.image_size.width,
                        p / in_plan.image_size.width,
                    };
                    const color pixel_color = this->render_pixel(in_plan, pixel, inverse_image_size);
                    pixels[p] = rgba{ to_rgb(pixel_color), 255 };

                    std::lock_guard lock{ this->progress_mtx };
                    this->progress += 100.f * inverse_pixel_count;
                    std::cout << "\rRendering image fragments... " << std::fixed << std::setprecision(2) << this->progress << "%" << std::flush;
                }
            }));
        }
    }

    std::cout << "\rRendering image fragments... Done.  " << std::endl;
    return pixels;
}

color renderer_cpu::render_single_pixel(const render_plan& in_plan, const pixel_position& in_position) const
{
    return this->render_pixel(in_plan, in_position, { 1.f / in_plan.image_size.width, 1.f / in_plan.image_size.height });
}

color renderer_cpu::render_pixel(const render_plan& in_plan, const pixel_position& in_position,
    const extent_2D<float>& in_inverse_size) const
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