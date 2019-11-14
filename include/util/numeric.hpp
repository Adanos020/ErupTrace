#pragma once

#include <util/pairs.hpp>

#include <limits>

using array_index = size_t;

constexpr float infinity = std::numeric_limits<float>::infinity();

inline static float normalize(const float in_value, const min_max<float>& in_range)
{
    return (in_value - in_range.min) / (in_range.max - in_range.min);
}

inline static float map(const float in_value, const from_to<min_max<float>>& in_ranges)
{
    return in_ranges.to.min + ((in_ranges.to.max - in_ranges.to.min) * normalize(in_value, in_ranges.from));
}