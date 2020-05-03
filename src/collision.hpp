#pragma once
#ifndef COLLISION_HPP
#define COLLISION_HPP

#include "Utility/vec.hpp"
#include <tuple>

bool areColliding_SphereVsBox(
    const vec2& posA, const float& radA, const vec2& posB,
    const vec2& scaleB) noexcept;

vec2 lineLineIntersection(
    const vec2& A, const vec2& B, const vec2& C, const vec2& D) noexcept;

#endif // COLLISION_HPP