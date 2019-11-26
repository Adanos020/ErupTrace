#pragma once

#include <util/pairs.hpp>

#include <limits>

using array_index = size_t;

template<typename T = float>
static constexpr T infinity = std::numeric_limits<T>::infinity();

template<typename T>
inline static bool is_clamped(const T in_value, const min_max<T>& in_range)
{
    return in_value >= in_range.min && in_value <= in_range.max;
}

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
inline static T unit_mirrored_repeat(const T in_value)
{
    static_assert(std::is_floating_point_v<T>);

    float i;
    const float fract = glm::modf(in_value, i);
    if (glm::mod(i, 2.f) != 0.f)
    {
        return 1.f - fract;
    }
    return fract;
}

template<typename T>
inline static T mirrored_repeat(const T in_value, const min_max<T>& in_range)
{
    static_assert(std::is_floating_point_v<T>);
    const float range_size = in_range.max - in_range.min;
    return in_range.min + (unit_mirrored_repeat(normalize(glm::abs(in_value), in_range)) * range_size);
}

template<typename T>
inline static T repeat(const T in_value, const min_max<T>& in_range)
{
    static_assert(std::is_floating_point_v<T>);
    return in_range.min + glm::mod(in_value - in_range.min, in_range.max - in_range.min);
}