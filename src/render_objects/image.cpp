#include <render_objects/image.hpp>

#include <util/barycentric.hpp>
#include <util/interpolation.hpp>
#include <util/numeric.hpp>
#include <util/vector.hpp>

// Wrapping

static bool is_normalized(const barycentric_2D& in_mapping)
{
    return !(in_mapping.U < 0.f || in_mapping.U > 1.f || in_mapping.V < 0.f || in_mapping.V > 1.f);
}

static barycentric_2D wrap_clamp_to_edge(const barycentric_2D& in_mapping)
{
    return barycentric_2D{ glm::clamp(in_mapping.U, 0.f, 1.f), glm::clamp(in_mapping.V, 0.f, 1.f) };
}

static barycentric_2D wrap_mirrored_repeat(const barycentric_2D& in_mapping)
{
    float U_i, V_i;
    barycentric_2D final_mapping = { glm::modf(in_mapping.U, U_i), glm::modf(in_mapping.V, V_i) };
    if (glm::mod(U_i, 2.f) != 0.f)
    {
        final_mapping.U = 1.f - final_mapping.U;
    }
    if (glm::mod(V_i, 2.f) != 0.f)
    {
        final_mapping.V = 1.f - final_mapping.V;
    }
    return final_mapping;
}

static barycentric_2D wrap_repeat(const barycentric_2D& in_mapping)
{
    return barycentric_2D{ glm::fract(in_mapping.U), glm::fract(in_mapping.V) };
}

bool wrap(barycentric_2D& out_mapping, const image::wrap_method in_wrap_method)
{
    switch (in_wrap_method)
    {
        case image::wrap_method::clamp_to_border:
        {
            return !is_normalized(out_mapping);
        }
        case image::wrap_method::clamp_to_edge:
        {
            out_mapping = wrap_clamp_to_edge(out_mapping);
            break;
        }
        case image::wrap_method::mirrored_repeat:
        {
            out_mapping = wrap_mirrored_repeat(out_mapping);
            break;
        }
        case image::wrap_method::repeat:
        {
            out_mapping = wrap_repeat(out_mapping);
            break;
        }
        default: break;
    }
    return true;
}

// Filtering

static color filter_cubic(const image& in_sampled_image, const barycentric_2D& in_mapping,
    const image::wrap_method in_wrap_method, const min_max<texture_position_2D>& in_image_fragment)
{
    const std::array<color, 16> neighbors = {

    };
    const float U = in_mapping.U;
    const float V = in_mapping.V;
    return bicubic(neighbors, U, V);
}

static color filter_linear(const image& in_sampled_image, const barycentric_2D& in_mapping,
    const image::wrap_method in_wrap_method, const min_max<texture_position_2D>& in_image_fragment)
{
    const uint32_t width = in_image_fragment.max.s - in_image_fragment.min.s;
    const uint32_t height = in_image_fragment.max.t - in_image_fragment.min.t;

    const texture_position_2D texcoord = {
        in_image_fragment.min.s + in_mapping.U * (width - 1),
        in_image_fragment.min.t + in_mapping.V * (height - 1),
    };

    const uint32_t left = glm::round(texcoord.s);
    const uint32_t up = glm::round(texcoord.t);
    const uint32_t right = left + 1;
    const uint32_t down = up + 1;

    const min_max<barycentric_2D> nearest = {
        barycentric_2D{
            float(left - in_image_fragment.min.s) / width,
            float(up - in_image_fragment.min.t) / height,
        },
        barycentric_2D{
            float(right - in_image_fragment.min.s) / width,
            float(down - in_image_fragment.min.t) / height,
        },
    };

    const std::array<color, 4> neighbors = {
        in_sampled_image.pixels[left + up * in_sampled_image.size.width],
        in_sampled_image.pixels[right + up * in_sampled_image.size.width],
        in_sampled_image.pixels[left + down * in_sampled_image.size.width],
        in_sampled_image.pixels[right + down * in_sampled_image.size.width],
    };

    const float U = normalize(in_mapping.U, { nearest.min.U, nearest.max.U });
    const float V = normalize(in_mapping.V, { nearest.min.V, nearest.max.V });
    return bilerp(neighbors, U, V);
}

static color filter_nearest(const image& in_sampled_image, const barycentric_2D& in_mapping,
    const image::wrap_method in_wrap_method, const min_max<texture_position_2D>& in_image_fragment)
{
    barycentric_2D final_mapping = in_mapping;
    if (wrap(final_mapping, in_wrap_method))
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

color filter(const image& in_sampled_image, const barycentric_2D& in_mapping, const min_max<texture_position_2D>& in_image_fragment,
    const image::wrap_method in_wrap_method, const image::filtering_method in_filtering_method)
{
    switch (in_filtering_method)
    {
        case image::filtering_method::cubic:   return filter_cubic(in_sampled_image, in_mapping, in_wrap_method, in_image_fragment);
        case image::filtering_method::linear:  return filter_linear(in_sampled_image, in_mapping, in_wrap_method, in_image_fragment);
        case image::filtering_method::nearest: return filter_nearest(in_sampled_image, in_mapping, in_wrap_method, in_image_fragment);
    }
    return black;
}