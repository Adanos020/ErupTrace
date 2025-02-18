#pragma once

#include <util/colors.hpp>
#include <util/vector.hpp>

#include <glm/gtx/optimum_pow.hpp>

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

inline static displacement_3D random_in_unit_sphere()
{
    while (true)
    {
        if (const displacement_3D dir = random_point_in_unit_cube(); glm::dot(dir, dir) < 1.f)
        {
            return dir;
        }
    }
}

inline static direction_3D random_on_unit_sphere()
{
    return glm::normalize(random_point_in_unit_cube());
}

inline static direction_3D random_cosine_direction()
{
    const float r_1 = random_uniform<float>();
    const float r_2 = random_uniform<float>();
    const float sqrt_r_2 = glm::sqrt(r_2);
    const float phi = glm::two_pi<float>() * r_1;
    return direction_3D{
        glm::cos(phi) * sqrt_r_2,
        glm::sin(phi) * sqrt_r_2,
        glm::sqrt(1 - r_2),
    };
}

inline static direction_3D random_to_sphere(const float in_radius, const float in_square_distance)
{
    const float r_1 = random_uniform<float>();
    const float r_2 = random_uniform<float>();
    const float z = 1.f + (r_2 * (glm::sqrt(1.f - (glm::pow2(in_radius) / in_square_distance)) - 1.f));
    const float phi = glm::two_pi<float>() * r_1;
    const float sqrt_1_minus_square_z = glm::sqrt(1.f - glm::pow2(z));
    return direction_3D{
        glm::cos(phi) * sqrt_1_minus_square_z,
        glm::sin(phi) * sqrt_1_minus_square_z,
        glm::sqrt(1 - r_2),
    };
}

inline static position_3D random_in_unit_disk(const direction_3D& in_axis = z_axis)
{
    return (direction_3D{ 1.f } - in_axis) * random_in_unit_sphere();
}