#pragma once

#include <util/colors.hpp>
#include <util/vector_types.hpp>

#include <chrono>
#include <random>
#include <type_traits>

inline static bool random_chance(const float in_probability = 0.5f)
{
    static const auto seed = uint32_t(std::chrono::system_clock::now().time_since_epoch().count());
    static std::default_random_engine rng{ seed };
    std::bernoulli_distribution distribution{ in_probability };
    return distribution(rng);
}

template <typename T>
inline static auto random_uniform(const T in_min = T(0), const T in_max = T(1))
{
    static_assert(std::is_arithmetic_v<T>);

    static const auto seed = uint32_t(std::chrono::system_clock::now().time_since_epoch().count());
    static std::default_random_engine rng{ seed };

    if constexpr (std::is_integral_v<T>)
    {
        std::uniform_int_distribution<T> distribution{ in_min, in_max };
        return distribution(rng);
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        std::uniform_real_distribution<T> distribution{ in_min, in_max };
        return distribution(rng);
    }
}

inline static color random_color()
{
    return color{ random_uniform<float>(), random_uniform<float>(), random_uniform<float>() };
}

inline static displacement_3d random_direction()
{
    while (true)
    {
        const displacement_3d dir = {
            random_uniform(-1.f, 1.f), random_uniform(-1.f, 1.f), random_uniform(-1.f, 1.f)
        };
        if (const float l = glm::length(dir); l * l < 1.f)
        {
            return dir;
        }
    }
}

inline static displacement_3d random_in_unit_disk(const direction_3d& in_axis = z_axis)
{
    while (true)
    {
        if (const displacement_3d dir = (direction_3d{ 1.f } - in_axis) * random_direction(); glm::dot(dir, dir) < 1.f)
        {
            return dir;
        }
    }
}