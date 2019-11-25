#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/spline.hpp>
#include <glm/gtc/constants.hpp>

#include <array>

// 2D

template<typename T = float>
inline static T bicatrom(const std::array<T, 16>& in_values, const float in_u, const float in_v)
{
    const T v_1 = glm::catmullRom(in_values[0],  in_values[1],  in_values[2],  in_values[3],  in_u);
    const T v_2 = glm::catmullRom(in_values[4],  in_values[5],  in_values[6],  in_values[7],  in_u);
    const T v_3 = glm::catmullRom(in_values[8],  in_values[9],  in_values[10], in_values[11], in_u);
    const T v_4 = glm::catmullRom(in_values[12], in_values[13], in_values[14], in_values[15], in_u);
    return glm::catmullRom(v_1, v_2, v_3, v_4, in_v);
}

template<typename T = float>
inline static T bilerp(const std::array<T, 4>& in_values, const float in_u, const float in_v)
{
    const T fxy_1 = glm::mix(in_values[0], in_values[1], in_u);
    const T fxy_2 = glm::mix(in_values[2], in_values[3], in_u);
    return glm::mix(fxy_1, fxy_2, in_v);
}

// 3D

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