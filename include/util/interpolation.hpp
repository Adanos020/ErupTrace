#pragma once

#include <glm/glm.hpp>

#include <array>

template<typename T = float>
inline static T bilerp(const std::array<T, 4>& in_values, const float in_u, const float in_v)
{
    const T fxy_1 = glm::mix(in_values[0], in_values[1], in_u);
    const T fxy_2 = glm::mix(in_values[2], in_values[3], in_u);
    return glm::mix(fxy_1, fxy_2, in_v);
}

template<typename T = float>
inline static T bicubic(const std::array<T, 16>& in_values, const float in_u, const float in_v)
{
    return T{ 0.f };
}

template<typename T = float>
inline static T trilerp(const std::array<T, 8>& in_values, const float in_u, const float in_v, const float in_w)
{
    float accumulated = 0.f;
    for (size_t it = 0; it < in_values.size(); ++it)
    {
        const size_t i = (it & 4) >> 2;
        const size_t j = (it & 2) >> 1;
        const size_t k = (it & 1) >> 0;
        accumulated +=
            (i * in_u + (1 - i) * (1 - in_u)) *
            (j * in_v + (1 - j) * (1 - in_v)) *
            (k * in_w + (1 - k) * (1 - in_w)) * in_values[it];
    }
    return accumulated;
}

inline static float perlin_trilerp(const std::array<glm::vec3, 8>& in_values, const float in_u, const float in_v, const float in_w)
{
    const float uu = in_u * in_u * (3.f - 2.f * in_u);
    const float vv = in_v * in_v * (3.f - 2.f * in_v);
    const float ww = in_w * in_w * (3.f - 2.f * in_w);

    float accumulated = 0.f;
    for (size_t it = 0; it < in_values.size(); ++it)
    {
        const size_t i = (it & 4) >> 2;
        const size_t j = (it & 2) >> 1;
        const size_t k = (it & 1) >> 0;
        const glm::vec3 weight = { in_u - i, in_v - j, in_w - k };
        accumulated +=
            (i * uu + (1 - i) * (1 - uu)) *
            (j * vv + (1 - j) * (1 - vv)) *
            (k * ww + (1 - k) * (1 - ww)) * glm::dot(in_values[it], weight);
    }
    return accumulated;
}