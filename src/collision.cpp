#include "collision.hpp"
#include <cassert>
#include <limits>

bool areColliding_SphereVsBox(
    const vec2& posA, const float& radA, const vec2& posB,
    const vec2& scaleB) noexcept {
    const auto sphereLeft = posA.x() - radA;
    const auto sphereRight = posA.x() + radA;
    const auto sphereBottom = posA.y() - radA;
    const auto sphereTop = posA.y() + radA;
    const auto boxLeft = posB.x() - scaleB.x();
    const auto boxRight = posB.x() + scaleB.x();
    const auto boxBottom = posB.y() - scaleB.y();
    const auto boxTop = posB.y() + scaleB.y();

    return (
        sphereRight > boxLeft && sphereLeft < boxRight &&
        sphereTop > boxBottom && sphereBottom < boxTop);
}

vec2 lineLineIntersection(
    const vec2& A, const vec2& B, const vec2& C, const vec2& D) noexcept {
    // Line AB represented as a1x + b1y = c1
    const float a1 = B.y() - A.y();
    const float b1 = A.x() - B.x();
    const float c1 = a1 * (A.x()) + b1 * (A.y());

    // Line CD represented as a2x + b2y = c2
    const float a2 = D.y() - C.y();
    const float b2 = C.x() - D.x();
    const float c2 = a2 * (C.x()) + b2 * (C.y());

    if (const float determinant = a1 * b2 - a2 * b1; determinant == 0) {
        // The lines are parallel. This is simplified
        return vec2(std::numeric_limits<float>::max());
    } else {
        const float x = (b2 * c1 - b1 * c2) / determinant;
        const float y = (a1 * c2 - a2 * c1) / determinant;
        return vec2(x, y);
    }
}