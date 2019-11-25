#include <render_objects/image.hpp>

#include <util/barycentric.hpp>
#include <util/interpolation.hpp>
#include <util/numeric.hpp>
#include <util/vector.hpp>

// Wrapping

static float wrap(const float in_value, const min_max<float>& in_range, const image::wrap_method in_wrap_method)
{
    switch (in_wrap_method)
    {
        case image::wrap_method::clamp_to_border: return is_clamped(in_value, { in_range.min, in_range.max }) ? in_value : -1.f;
        case image::wrap_method::clamp_to_edge:   return glm::clamp(in_value, in_range.min, in_range.max);
        case image::wrap_method::mirrored_repeat: return mirrored_repeat(in_value, in_range);
        case image::wrap_method::repeat:          return repeat(in_value, in_range);
    }
    return -1.f;
}

static barycentric_2D wrap(const barycentric_2D& in_mapping, const image::wrap_method in_wrap_method)
{
    return barycentric_2D{
        wrap(in_mapping.U, { 0.f, 1.f }, in_wrap_method),
        wrap(in_mapping.V, { 0.f, 1.f }, in_wrap_method),
    };
}

// Filtering

static auto pick_4x4_neighbors_and_interpolants(const image& in_sampled_image,
    const min_max<texture_position_2D>& in_image_fragment, const barycentric_2D& in_mapping,
    const image::wrap_method in_wrap_method)
{
    const auto wrap_s_in_fragment = [&](const float s) {
        return wrap(s, { in_image_fragment.min.s, in_image_fragment.max.s - 1 }, in_wrap_method);
    };
    const auto wrap_t_in_fragment = [&](const float t) {
        return wrap(t, { in_image_fragment.min.t, in_image_fragment.max.t - 1 }, in_wrap_method);
    };

    const float fragment_width = in_image_fragment.max.s - in_image_fragment.min.s;
    const float fragment_height = in_image_fragment.max.t - in_image_fragment.min.t;

    const texture_position_2D texcoord = {
        wrap_s_in_fragment(in_image_fragment.min.s - 0.5f + (in_mapping.U * fragment_width)),
        wrap_t_in_fragment(in_image_fragment.min.t - 0.5f + (in_mapping.V * fragment_height)),
    };

    float s_int, t_int;
    const float s_fract = glm::modf(texcoord.s, s_int);
    const float t_fract = glm::modf(texcoord.t, t_int);

    std::array<float, 8> neighbors;
    for (size_t i = 0; i < 4; ++i)
    {
        neighbors[i + 0] = wrap_s_in_fragment(s_int - 1.f + float(i));
        neighbors[i + 4] = wrap_t_in_fragment(t_int - 1.f + float(i));
    }

    std::array<color, 16> texels;
    for (size_t t = 4; t < 8; ++t)
    {
        for (size_t s = 0; s < 4; ++s)
        {
            const size_t texel_index = s + ((t - 4) * 4);
            if (neighbors[s] < 0.f || neighbors[t] < 0.f)
            {
                texels[texel_index] = black;
            }
            else
            {
                const size_t pixel_index = neighbors[s] + (neighbors[t] * in_sampled_image.size.width);
                texels[texel_index] = in_sampled_image.pixels[pixel_index];
            }
        }
    }

    return std::make_tuple(texels, s_fract, t_fract);
}

static auto pick_2x2_neighbors_and_interpolants(const image& in_sampled_image,
    const min_max<texture_position_2D>& in_image_fragment, const barycentric_2D& in_mapping,
    const image::wrap_method in_wrap_method)
{
    const auto wrap_s_in_fragment = [&](const float s) {
        return wrap(s, { in_image_fragment.min.s, in_image_fragment.max.s - 1 }, in_wrap_method);
    };
    const auto wrap_t_in_fragment = [&](const float t) {
        return wrap(t, { in_image_fragment.min.t, in_image_fragment.max.t - 1 }, in_wrap_method);
    };

    const float fragment_width = in_image_fragment.max.s - in_image_fragment.min.s;
    const float fragment_height = in_image_fragment.max.t - in_image_fragment.min.t;

    const texture_position_2D texcoord = {
        wrap_s_in_fragment(in_image_fragment.min.s - 0.5f + (in_mapping.U * fragment_width)),
        wrap_t_in_fragment(in_image_fragment.min.t - 0.5f + (in_mapping.V * fragment_height)),
    };

    float s_int, t_int;
    const float s_fract = glm::modf(texcoord.s, s_int);
    const float t_fract = glm::modf(texcoord.t, t_int);

    const float left = wrap_s_in_fragment(s_int);
    const float up = wrap_t_in_fragment(t_int);
    const float right = wrap_s_in_fragment(left + 1.f);
    const float down = wrap_t_in_fragment(up + 1.f);

    const std::array<color, 4> texels = {
        (left  < 0.f || up   < 0.f) ? black : in_sampled_image.pixels[size_t(left + up * in_sampled_image.size.width)],
        (right < 0.f || up   < 0.f) ? black : in_sampled_image.pixels[size_t(right + up * in_sampled_image.size.width)],
        (left  < 0.f || down < 0.f) ? black : in_sampled_image.pixels[size_t(left + down * in_sampled_image.size.width)],
        (right < 0.f || down < 0.f) ? black : in_sampled_image.pixels[size_t(right + down * in_sampled_image.size.width)],
    };

    return std::make_tuple(texels, s_fract, t_fract);
}

color filter_catrom(const image& in_sampled_image, const min_max<texture_position_2D>& in_image_fragment,
    const barycentric_2D& in_mapping, const image::wrap_method in_wrap_method)
{
    const auto [neighbors, U, V] = pick_4x4_neighbors_and_interpolants(
        in_sampled_image, in_image_fragment, in_mapping, in_wrap_method);
    return bicatrom(neighbors, U, V);
}

color filter_linear(const image& in_sampled_image, const min_max<texture_position_2D>& in_image_fragment,
    const barycentric_2D& in_mapping, const image::wrap_method in_wrap_method)
{
    const auto [neighbors, U, V] = pick_2x2_neighbors_and_interpolants(
        in_sampled_image, in_image_fragment, in_mapping, in_wrap_method);
    return bilerp(neighbors, U, V);
}

color filter_nearest(const image& in_sampled_image, const min_max<texture_position_2D>& in_image_fragment,
    const barycentric_2D& in_mapping, const image::wrap_method in_wrap_method)
{
    if (const barycentric_2D final_mapping = wrap(in_mapping, in_wrap_method);
        final_mapping.U >= 0.f && final_mapping.V >= 0.f)
    {
        const uint32_t width = in_image_fragment.max.s - in_image_fragment.min.s;
        const uint32_t height = in_image_fragment.max.t - in_image_fragment.min.t;
        const pixel_position nearest = {
            in_image_fragment.min.s + final_mapping.U * width,
            in_image_fragment.min.t + final_mapping.V * height,
        };
        return in_sampled_image.pixels[nearest.x + (nearest.y * in_sampled_image.size.width)];
    }
    return black;
}