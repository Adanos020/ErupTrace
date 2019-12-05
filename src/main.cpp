#include <render_objects/render_plan.hpp>
#include <renderer_cpu/renderer_cpu.hpp>
#include <util/string.hpp>

#include <external/stb_image_write.h>

#include <iostream>
#include <string>

#ifdef NDEBUG
#   define THREAD_COUNT 20
#else
#   define THREAD_COUNT 1
#   define SINGLE_PIXEL_TEST 0
#endif

using namespace std::string_literals;

void export_image(const std::vector<rgba>& image, const extent_2D<uint32_t> image_size, const std::string_view path)
{
    std::cout << "Writing to file... ";

    constexpr uint32_t channels = 4;
    if (string_ends_with(path, ".png"))
    {
        stbi_write_png(path.data(), image_size.width, image_size.height, channels, image.data(),
            image_size.width * channels);
    }
    else if (string_ends_with(path, ".jpg"))
    {
        constexpr int32_t quality = 100;
        stbi_write_jpg(path.data(), image_size.width, image_size.height, channels, image.data(), quality);
    }
    else
    {
        const size_t last_dot_pos = path.find_last_of('.');
        const std::string_view format = path.substr(last_dot_pos + 1, path.size() - last_dot_pos - 1);
        throw std::runtime_error("Unsupported image format: "s + format.data());
    }

    std::cout << "Done." << std::endl;
}

int main()
{
    try
    {
        const extent_2D<uint32_t> image_size = { 500, 500 };
        const render_plan plan = render_plan::cornell_box(image_size);
        renderer_cpu renderer{ 500, THREAD_COUNT };
#if SINGLE_PIXEL_TEST
        const pixel_position pixel_pos = { 254, 400 };
        const color pixel = renderer.render_single_pixel(plan, pixel_pos);
        std::cout << "Rendered color {" << pixel.r << " " << pixel.g << " " << pixel.b
            << "} at pixel {" << pixel_pos.x << " " << pixel_pos.y << "}" << std::endl;
#else
        const std::vector<rgba> image = renderer.render_scene(plan);
        export_image(image, image_size, "test.png");
#endif
    }
    catch (const std::exception& e)
    {
        std::cerr << "[std::exception] " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}