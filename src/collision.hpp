#pragma once
#ifndef COLLISION_HPP
#define COLLISION_HPP

#include "Utility/vec.hpp"
#include <tuple>

bool areColliding_SphereVsBox(
    const vec2& posA, const float& radA, const vec2& posB,
    const vec2& scaleB) noexcept;

std::tuple<bool, vec2, vec2> rayBBoxIntersection(
    const vec2& rayPos, const vec2& rayDir, const vec2& boxCenter,
    const vec2& boxExtents, const bool projectBackwards = false) noexcept;
#endif // COLLISION_HPP