#pragma once
#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "ecsComponent.hpp"
#include "particle.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

///////////////////////////////////////////////////////////////////////////
/// \class  ParticleComponent
struct ParticleComponent final : public ecsComponent<ParticleComponent> {
    Game_Particle particle;
};

///////////////////////////////////////////////////////////////////////////
/// \class  PhysicsComponent
struct PhysicsComponent final : public ecsComponent<PhysicsComponent> {
    float mass = 1.0F;
    float inv_mass = 1.0F;
    float restitution = 0.5F;
};

///////////////////////////////////////////////////////////////////////////
/// \class  BoundingBoxComponent
struct BoundingBoxComponent final : public ecsComponent<BoundingBoxComponent> {
    vec2 extents;
};

///////////////////////////////////////////////////////////////////////////
/// \class  MoveableComponent
struct MoveableComponent final : public ecsComponent<MoveableComponent> {};

#endif // COMPONENTS_HPP