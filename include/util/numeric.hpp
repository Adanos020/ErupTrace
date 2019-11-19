#pragma once

#include <util/pairs.hpp>

#include <limits>

using array_index = size_t;

template<typename T = float>
static constexpr T infinity = std::numeric_limits<T>::infinity();

template<typename T>
inline static T normalize(const T in_value, const min_max<T>& in_range)
{
    static_assert(std::is_floating_point_v<T>);
    return (in_value - in_range.min) / (in_range.max - in_range.min);
}

template<typename T>
inline static T map(const T in_value, const from_to<min_max<T>>& in_ranges)
{
    static_assert(std::is_floating_point_v<T>);
    return in_ranges.to.min + ((in_ranges.to.max - in_ranges.to.min) * normalize(in_value, in_ranges.from));
}

template<typename T>
inline static T wrap_clamp_to_border(const T in_value, const min_max<T>& in_range)
{
    static_assert(std::is_floating_point_v<T>);
    return (in_value < in_range.min || in_value > in_range.max) ? -1.f : in_value;
}

template<typename T>
inline static T wrap_clamp_to_edge(const T in_value, const min_max<T>& in_range)
{
    static_assert(std::is_floating_point_v<T>);
    return glm::clamp(in_value, in_range.min, in_range.max);
}

template<typename T>
inline static T wrap_mirrored_repeat(const T in_value, const min_max<T>& in_range)
{
    static_assert(std::is_floating_point_v<T>);

    float i;
    const float final_value = glm::modf(in_value, i);
    if (glm::mod(i, 2.f) != 0.f)
    {
        return 1.f - final_value;
    }
    return final_value;
}

template<typename T>
inline static T wrap_repeat(const T in_value, const min_max<T>& in_range)
{
    static_assert(std::is_floating_point_v<T>);
    return map(glm::fract(normalize(in_value, in_range)), { { 0.f, 1.f }, in_range });
}