#include <render_objects/image.hpp>

#include <util/barycentric.hpp>
#include <util/interpolation.hpp>
#include <util/numeric.hpp>
#include <util/vector.hpp>

// Wrapping

static barycentric_2D wrap(const barycentric_2D& in_mapping, const image::wrap_method in_wrap_method)
{
    switch (in_wrap_method)
    {
    case image::wrap_method::clamp_to_border:
        return barycentric_2D{
            wrap_clamp_to_border(in_mapping.U, { 0.f, 1.f }),
            wrap_clamp_to_border(in_mapping.V, { 0.f, 1.f }),
        };
    case image::wrap_method::clamp_to_edge:
        return barycentric_2D{
            wrap_clamp_to_edge(in_mapping.U, { 0.f, 1.f }),
            wrap_clamp_to_edge(in_mapping.V, { 0.f, 1.f }),
        };
    case image::wrap_method::mirrored_repeat:
        return barycentric_2D{
            wrap_mirrored_repeat(in_mapping.U, { 0.f, 1.f }),
            wrap_mirrored_repeat(in_mapping.V, { 0.f, 1.f }),
        };
    case image::wrap_method::repeat:
        return barycentric_2D{
            wrap_repeat(in_mapping.U, { 0.f, 1.f }),
            wrap_repeat(in_mapping.V, { 0.f, 1.f }),
        };
    }
    return barycentric_2D{ -1.f, -1.f };
}

static float wrap(const float in_value, const min_max<float>& in_range, const image::wrap_method in_wrap_method)
{
    switch (in_wrap_method)
    {
        case image::wrap_method::clamp_to_border: return wrap_clamp_to_border(in_value, in_range);
        case image::wrap_method::clamp_to_edge:   return wrap_clamp_to_edge(in_value, in_range);
        case image::wrap_method::mirrored_repeat: return wrap_mirrored_repeat(in_value, in_range);
        case image::wrap_method::repeat:          return wrap_repeat(in_value, in_range);
    }
    return -1.f;
}

// Filtering

color filter_cubic(const image& in_sampled_image, const min_max<texture_position_2D>& in_image_fragment,
    const barycentric_2D& in_mapping, const image::wrap_method in_wrap_method)
{
    const std::array<color, 16> neighbors = {

    };
    const float U = in_mapping.U;
    const float V = in_mapping.V;
    return bicubic(neighbors, U, V);
}

color filter_linear(const image& in_sampled_image, const min_max<texture_position_2D>& in_image_fragment,
    const barycentric_2D& in_mapping, const image::wrap_method in_wrap_method)
{
    const float width = in_image_fragment.max.s - in_image_fragment.min.s;
    const float height = in_image_fragment.max.t - in_image_fragment.min.t;
    const float inverse_width = 1.f / (width - 1.f);
    const float inverse_height = 1.f / (height - 1.f);

    const texture_position_2D texcoord = {
        in_image_fragment.min.s + in_mapping.U * (width - 1),
        in_image_fragment.min.t + in_mapping.V * (height - 1),
    };

    const float left = wrap(glm::floor(texcoord.s), { in_image_fragment.min.s, in_image_fragment.max.s }, in_wrap_method);
    const float up = wrap(glm::floor(texcoord.t), { in_image_fragment.min.t, in_image_fragment.max.t }, in_wrap_method);
    const float right = wrap(left + 1.f, { in_image_fragment.min.s, in_image_fragment.max.s }, in_wrap_method);
    const float down = wrap(up + 1.f, { in_image_fragment.min.t, in_image_fragment.max.t }, in_wrap_method);

    const min_max<barycentric_2D> nearest = {
        barycentric_2D{
            (left - in_image_fragment.min.s) * inverse_width,
            (up - in_image_fragment.min.t) * inverse_height,
        },
        barycentric_2D{
            (right - in_image_fragment.min.s) * inverse_width,
            (down - in_image_fragment.min.t) * inverse_height,
        },
    };

    const std::array<color, 4> neighbors = {
        in_sampled_image.pixels[size_t(left + up * in_sampled_image.size.width)],
        in_sampled_image.pixels[size_t(right + up * in_sampled_image.size.width)],
        in_sampled_image.pixels[size_t(left + down * in_sampled_image.size.width)],
        in_sampled_image.pixels[size_t(right + down * in_sampled_image.size.width)],
    };

    const float U = normalize(
        wrap(in_mapping.U, { 0.f, 1.f }, in_wrap_method),
        { nearest.min.U, nearest.max.U });
    const float V = normalize(
        wrap(in_mapping.V, { 0.f, 1.f }, in_wrap_method),
        { nearest.min.V, nearest.max.V });
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
            in_image_fragment.min.s + final_mapping.U * (width - 1),
            in_image_fragment.min.t + final_mapping.V * (height - 1),
        };
        return in_sampled_image.pixels[nearest.x + (nearest.y * in_sampled_image.size.width)];
    }
    return black;
}