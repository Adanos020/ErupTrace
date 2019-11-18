#pragma once

#include <util/colors.hpp>
#include <util/vector.hpp>

#include <chrono>
#include <random>
#include <type_traits>

inline static bool random_chance(const float in_probability = 0.5f)
{
#if NDEBUG
    static const auto seed = uint32_t(std::chrono::system_clock::now().time_since_epoch().count());
#else
    static const uint32_t seed = 2137;
#endif
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

inline static position_3D random_point_in_unit_cube()
{
    return position_3D{ random_uniform(-1.f, 1.f), random_uniform(-1.f, 1.f), random_uniform(-1.f, 1.f) };
}

inline static displacement_3D random_direction()
{
    while (true)
    {
        if (const displacement_3D dir = random_point_in_unit_cube(); glm::dot(dir, dir) < 1.f)
        {
            return dir;
        }
    }
}

inline static position_3D random_in_unit_disk(const direction_3D& in_axis = z_axis)
{
    return (direction_3D{ 1.f } - in_axis) * random_direction();
}