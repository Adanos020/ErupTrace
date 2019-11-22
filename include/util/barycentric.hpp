#pragma once

#include <glm/glm.hpp>

#include <tuple>

struct barycentric_2D
{
    float U, V;

    constexpr barycentric_2D(const float U = 0.f, const float V = 0.f)
        : U(U), V(V)
    {
    }

    constexpr barycentric_2D(const std::pair<float, float>& pair)
        : barycentric_2D(pair.first, pair.second)
    {
    }

    constexpr operator glm::vec2() const
    {
        return { this->U, this->V };
    }
};

struct barycentric_3D
{
    float U, V, W;

    constexpr barycentric_3D(const float U = 0.f, const float V = 0.f, const float W = 0.f)
        : U(U), V(V), W(W)
    {
    }

    constexpr barycentric_3D(const std::tuple<float, float, float>& triplet)
        : barycentric_3D(std::get<0>(triplet), std::get<1>(triplet), std::get<2>(triplet))
    {
    }

    constexpr operator glm::vec3() const
    {
        return { this->U, this->V, this->W };
    }
};