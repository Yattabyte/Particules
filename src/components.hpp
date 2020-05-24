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
    vec2 m_pos = vec2(0.0F);
    vec2 m_dimensions = vec2(1.0F);
    vec3 m_color = vec3(1.0F);
    float m_health = 1.0F;
};

///////////////////////////////////////////////////////////////////////////
/// \class  PhysicsComponent
struct PhysicsComponent final : public ecsComponent<PhysicsComponent> {
    vec2 m_velocity = vec2(0.0F);
    float mass = 1.0F;
    float inv_mass = 1.0F;
    float restitution = 0.5F;
    vec3 padding;
};

///////////////////////////////////////////////////////////////////////////
/// \class  CollisionManifoldComponent
struct CollisionManifoldComponent final
    : public ecsComponent<CollisionManifoldComponent> {
    struct CollisionManifold {
        std::shared_ptr<ecsEntity> otherEntity;
        vec2 normal = vec2(0.0F);
        float depth = 0.0F;
    };
    std::vector<CollisionManifold> collisions;
};

struct InnertComponent : public ecsComponent<InnertComponent> {};
struct MovingComponent final : public ecsComponent<MovingComponent> {};
struct FlammableComponent : public ecsComponent<FlammableComponent> {
    float wickTime = 1.0F; ///< How long it will burn for.
};
struct ExplosiveComponent : public ecsComponent<ExplosiveComponent> {
    float fuseTime = 1.0F; ///< How long it must burn until detonation.
};
struct OnFireComponent : public ecsComponent<OnFireComponent> {};

#endif // COMPONENTS_HPP