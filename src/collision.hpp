#pragma once
#ifndef COLLISION_HPP
#define COLLISION_HPP

#include "Utility/vec.hpp"
#include <tuple>

///////////////////////////////////////////////////////////////////////////
/// \brief  Checks if sphere is colliding with an AABB.
/// \param  posA    the position of the sphere.
/// \param  radA    the radius of the sphere.
/// \param  posB    ths position of the AABB.
/// \param  scale   the scale of the AABB.
/// \return true if the two are colliding, false otherwise.
bool areColliding_SphereVsBox(
    const vec2& posA, const float& radA, const vec2& posB,
    const vec2& scaleB) noexcept;

///////////////////////////////////////////////////////////////////////////
/// \brief  Checks if a ray is intersecting with an AABB.
/// \param  rayPos              the position of the ray in 2D space.
/// \param  rayDir              the ray direction in 2D space.
/// \param  boxCenter           the center position of the AABB.
/// \param  boxExtents          the scale of the AABB.
/// \param  projectBackwards    check if it intersects backwards.
/// \return tuple with if it intersected, where, and the normal.
std::tuple<bool, vec2, vec2> rayBBoxIntersection(
    const vec2& rayPos, const vec2& rayDir, const vec2& boxCenter,
    const vec2& boxExtents, const bool projectBackwards = false) noexcept;
#endif // COLLISION_HPP