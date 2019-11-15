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