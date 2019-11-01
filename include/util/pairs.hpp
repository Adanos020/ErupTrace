#pragma once

#include <iterator>
#include <utility>

template <typename T>
struct min_max
{
    T min;
    T max;

    constexpr min_max(const T& min = T{}, const T& max = T{})
        : min(min), max(max)
    {
    }

    constexpr min_max(const std::pair<const T&, const T&>& pair)
        : min(pair.first), max(pair.second)
    {
    }

    constexpr bool is_value_clamped(const T& value) const
    {
        return value >= min && value <= max;
    }
};

template <typename T>
struct from_to
{
    T from;
    T to;

    constexpr from_to(const T& from = T{}, const T& to = T{})
        : from(from), to(to)
    {
    }

    constexpr from_to(const std::pair<const T&, const T&>& pair)
        : from(pair.first), to(pair.second)
    {
    }
};

struct uv_mapping
{
    float u;
    float v;

    constexpr uv_mapping(const float u = 0.f, const float v = 0.f)
        : u(u), v(v)
    {
    }

    constexpr uv_mapping(const std::pair<float, float>& pair)
        : u(pair.first), v(pair.second)
    {
    }
};

template <typename T>
struct iterator_pair
{
    using iterator = typename T::iterator;

    iterator begin;
    iterator end;

    iterator_pair(iterator begin, iterator end)
        : begin(begin), end(end)
    {
    }

    iterator_pair(T& container)
        : iterator_pair(std::begin(container), std::end(container))
    {
    }
};

template <typename T>
struct const_iterator_pair
{
    using iterator = typename T::const_iterator;

    iterator begin;
    iterator end;

    const_iterator_pair(iterator begin, iterator end)
        : begin(begin), end(end)
    {
    }

    const_iterator_pair(const T& container)
        : const_iterator_pair(std::cbegin(container), std::cend(container))
    {
    }
};