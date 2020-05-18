#pragma once
#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "Utility/vec.hpp"
#include "ecsComponent.hpp"
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
    PARTICLE_TYPE m_type = PARTICLE_TYPE::CONCRETE;
    vec3 padding;
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
/// \class  MovingComponent
struct MovingComponent final : public ecsComponent<MovingComponent> {};

///////////////////////////////////////////////////////////////////////////
/// \class  CollisionManifoldComponent
struct CollisionManifoldComponent final
    : public ecsComponent<CollisionManifoldComponent> {
    struct CollisionManifold {
        EntityHandle otherEntity;
        vec2 normal = vec2(0.0F);
        float depth = 0.0F;
    };
    std::vector<CollisionManifold> collisions;
};

#endif // COMPONENTS_HPP