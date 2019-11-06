#pragma once

#include <type_traits>

template <typename T>
struct extent_2D
{
    static_assert(std::is_arithmetic_v<T>);

    constexpr extent_2D() = default;
    constexpr extent_2D(const T width, const T height)
        : width(width), height(height)
    {
    }

    constexpr float aspect() const
    {
        return float(this->width) / float(this->height);
    }

    T width = T{};
    T height = T{};
};

template <typename T>
struct extent_3D
{
    static_assert(std::is_arithmetic_v<T>);

    constexpr extent_3D() = default;
    constexpr extent_3D(const T width, const T height, const T depth)
        : width(width), height(height), depth(depth)
    {
    }

    T width;
    T height;
    T depth;
};