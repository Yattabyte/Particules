#pragma once
#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "Utility/vec.hpp"
#include "ecsComponent.hpp"
#include "ecsEntity.hpp"
#include "particle.hpp"
#include <vector>

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

///////////////////////////////////////////////////////////////////////////
/// \class  ParticleComponent
struct ParticleComponent final : public ecsComponent<ParticleComponent> {
    vec3 m_color = vec3(1.0F);
    vec2 m_pos = vec2(0.0F);
    float m_health = 1.0F;
    float m_density = 1.0f;
    bool m_useGravity = true;
    bool m_asleep = false;
};
constexpr auto qwe = sizeof(ParticleComponent);
///////////////////////////////////////////////////////////////////////////
/// \class  CollisionManifoldComponent
struct CollisionManifoldComponent final
    : public ecsComponent<CollisionManifoldComponent> {
    struct CollisionManifold {
        std::shared_ptr<ecsEntity> otherEntity;
        vec2 normal = vec2(0.0F);
    };
    std::vector<CollisionManifold> collisions;
};

struct FlammableComponent : public ecsComponent<FlammableComponent> {
    float wickTime = 1.0F; ///< How long it will burn for.
};
struct ExplosiveComponent : public ecsComponent<ExplosiveComponent> {
    float fuseTime = 1.0F; ///< How long it must burn until detonation.
};
struct OnFireComponent : public ecsComponent<OnFireComponent> {};
struct SpawnerComponent : public ecsComponent<SpawnerComponent> {};

#endif // COMPONENTS_HPP