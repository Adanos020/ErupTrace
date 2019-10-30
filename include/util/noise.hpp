#pragma once

#include <glm/glm.hpp>

#include <array>

class perlin
{
public:
    perlin();

    float noise(const glm::vec3&) const;

private:
    inline static constexpr size_t N_PERMUTATIONS = 256;
    std::array<glm::vec3, N_PERMUTATIONS> random_vectors;
    std::array<uint32_t, N_PERMUTATIONS> x_permutations;
    std::array<uint32_t, N_PERMUTATIONS> y_permutations;
    std::array<uint32_t, N_PERMUTATIONS> z_permutations;
};

inline static float turbulence(const perlin& in_noise, const glm::vec3& in_p, int in_depth = 7)
{
    float accumulate = 0.f;
    float weight = 1.f;
    glm::vec3 p_pow2 = in_p;

    for (; in_depth > 0; --in_depth)
    {
        accumulate += weight * in_noise.noise(p_pow2);
        weight *= 0.5f;
        p_pow2 *= 2.f;
    }
    return glm::abs(accumulate);
}