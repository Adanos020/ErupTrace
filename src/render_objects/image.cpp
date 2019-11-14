#include <render_objects/image.hpp>

#include <util/interpolation.hpp>
#include <util/numeric.hpp>

// Wrapping

static bool is_normalized(const UV_mapping& in_UV)
{
    return !(in_UV.U < 0.f || in_UV.U > 1.f || in_UV.V < 0.f || in_UV.V > 1.f);
}

static UV_mapping wrap_clamp_to_edge(const UV_mapping& in_UV)
{
    return UV_mapping{ glm::clamp(in_UV.U, 0.f, 1.f), glm::clamp(in_UV.V, 0.f, 1.f) };
}

static UV_mapping wrap_mirrored_repeat(const UV_mapping& in_UV)
{
    float U_i, V_i;
    UV_mapping final_UV = { glm::modf(in_UV.U, U_i), glm::modf(in_UV.V, V_i) };
    if (glm::mod(U_i, 2.f) != 0.f)
    {
        final_UV.U = 1.f - final_UV.U;
    }
    if (glm::mod(V_i, 2.f) != 0.f)
    {
        final_UV.V = 1.f - final_UV.V;
    }
    return final_UV;
}

static UV_mapping wrap_repeat(const UV_mapping& in_UV)
{
    return UV_mapping{ glm::fract(in_UV.U), glm::fract(in_UV.V) };
}

bool wrap(UV_mapping& out_UV, const image::wrap_method in_wrap_method)
{
    switch (in_wrap_method)
    {
        case image::wrap_method::clamp_to_border:
        {
            return !is_normalized(out_UV);
        }
        case image::wrap_method::clamp_to_edge:
        {
            out_UV = wrap_clamp_to_edge(out_UV);
            break;
        }
        case image::wrap_method::mirrored_repeat:
        {
            out_UV = wrap_mirrored_repeat(out_UV);
            break;
        }
        case image::wrap_method::repeat:
        {
            out_UV = wrap_repeat(out_UV);
            break;
        }
        default: break;
    }
    return true;
}

// Filtering

static color filter_cubic(const image& in_sampled_image, const UV_mapping& in_UV)
{
    const std::array<color, 16> neighbors = {

    };
    const float U = in_UV.U;
    const float V = in_UV.V;
    return bicubic(neighbors, U, V);
}

static color filter_linear(const image& in_sampled_image, const UV_mapping& in_UV)
{
    const uint32_t width = in_sampled_image.size.width;
    const uint32_t height = in_sampled_image.size.height;
    
    const UV_mapping delta = { 1.f / (width - 1), 1.f / (height - 1) };
    const min_max<UV_mapping> nearest = {
        UV_mapping{ delta.U * glm::floor(in_UV.U / delta.U), delta.V * glm::floor(in_UV.V / delta.V) },
        UV_mapping{ delta.U * glm::ceil(in_UV.U / delta.U), delta.V * glm::ceil(in_UV.V / delta.V) },
    };
    
    const uint32_t left  = map(nearest.min.U, { { 0.f, 1.f }, { 0, in_sampled_image.size.width - 1 } });
    const uint32_t right = map(nearest.max.U, { { 0.f, 1.f }, { 0, in_sampled_image.size.width - 1 } });
    const uint32_t up    = map(nearest.min.V, { { 0.f, 1.f }, { 0, in_sampled_image.size.height - 1 } });
    const uint32_t down  = map(nearest.max.V, { { 0.f, 1.f }, { 0, in_sampled_image.size.height - 1 } });

    const std::array<color, 4> neighbors = {
        in_sampled_image.pixels[left + up * width], in_sampled_image.pixels[right + up * width],
        in_sampled_image.pixels[left + down * width], in_sampled_image.pixels[right + down * width],
    };

    const float U = normalize(in_UV.U, { nearest.min.U, nearest.max.U });
    const float V = normalize(in_UV.V, { nearest.min.V, nearest.max.V });
    return bilerp(neighbors, U, V);
}

static color filter_nearest(const image& in_sampled_image, const UV_mapping& in_UV)
{
    const glm::ivec2 nearest = {
        glm::clamp<int32_t>(in_UV.U * in_sampled_image.size.width, 0.f, in_sampled_image.size.width - 1.f),
        glm::clamp<int32_t>(in_UV.V * in_sampled_image.size.height - 0.001f, 0.f, in_sampled_image.size.height - 1.f),
    };
    return in_sampled_image.pixels[nearest.x + (nearest.y * in_sampled_image.size.width)];
}

color filter(const image& in_sampled_image, const UV_mapping& in_UV, const image::filtering_method in_filtering_method)
{
    switch (in_filtering_method)
    {
        case image::filtering_method::cubic:   return filter_cubic(in_sampled_image, in_UV);
        case image::filtering_method::linear:  return filter_linear(in_sampled_image, in_UV);
        case image::filtering_method::nearest: return filter_nearest(in_sampled_image, in_UV);
    }
    return black;
}