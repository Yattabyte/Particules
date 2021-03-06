#include "collision.hpp"
#include <algorithm>

//////////////////////////////////////////////////////////////////////
/// areColliding_SphereVsBox
//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////
/// areColliding_BoxVsBox
//////////////////////////////////////////////////////////////////////

bool areColliding_BoxVsBox(
    const vec2& posA, const vec2& sclA, const vec2& posB,
    const vec2& sclB) noexcept {
    const bool x = std::abs(posA.data()[0] - posB.data()[0]) <=
                   (sclA.data()[0] + sclB.data()[0]);
    const bool y = std::abs(posA.data()[1] - posB.data()[1]) <=
                   (sclA.data()[1] + sclB.data()[1]);
    return x && y;

    /*const auto aLeft = posA.x() - sclA.x();
    const auto aRight = posA.x() + sclA.x();
    const auto aBot = posA.y() - sclA.y();
    const auto aTop = posA.y() + sclA.y();
    const auto bLeft = posB.x() - sclB.x();
    const auto bRight = posB.x() + sclB.x();
    const auto bBot = posB.y() - sclB.y();
    const auto bTop = posB.y() + sclB.y();
    return (aRight >= bLeft && aLeft <= bRight && aTop >= bBot && aBot <=
    bTop);*/
}

//////////////////////////////////////////////////////////////////////
/// rayBBoxIntersection
//////////////////////////////////////////////////////////////////////

std::tuple<bool, vec2, vec2, float> rayBBoxIntersection(
    const vec2& rayPos, const vec2& rayDir, const vec2& boxCenter,
    const vec2& boxExtents, const bool projectBackwards) noexcept {
    const vec2 boxMin = boxCenter - boxExtents;
    const vec2 boxMax = boxCenter + boxExtents;
    const vec2 invDir = vec2(1.0F) / rayDir;
    const auto tx1 = (boxMin.x() - rayPos.x()) * invDir.x();
    const auto tx2 = (boxMax.x() - rayPos.x()) * invDir.x();

    auto tmin = std::min(tx1, tx2);
    auto tmax = std::max(tx1, tx2);

    const auto ty1 = (boxMin.y() - rayPos.y()) * invDir.y();
    const auto ty2 = (boxMax.y() - rayPos.y()) * invDir.y();

    tmin = std::max(tmin, std::min(ty1, ty2));
    tmax = std::min(tmax, std::max(ty1, ty2));

    if (tmax >= tmin) {
        const auto& projectAmount = projectBackwards ? tmin : tmax;
        const vec2 intersectionPoint = rayPos + (rayDir * projectAmount);
        const vec2 p = intersectionPoint - boxCenter;
        const vec2 d = (boxMin - boxMax) * 0.5;
        constexpr float bias = 1.00001F;
        const vec2 normal =
            vec2(
                static_cast<float>(
                    static_cast<int>(p.x() / std::abs(d.x()) * bias)),
                static_cast<float>(
                    static_cast<int>(p.y() / std::abs(d.x()) * bias)))
                .normalize();
        return std::make_tuple(
            true, intersectionPoint, normal, std::abs(projectAmount));
    }
    return std::make_tuple(false, vec2(0), vec2(0), 0.0F);
}