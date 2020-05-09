#pragma once
#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "ecsComponent.hpp"
#include "particle.hpp"

///////////////////////////////////////////////////////////////////////////
/// \class  ParticleComponent
struct ParticleComponent final : public mini::ecsComponent<ParticleComponent> {
    Game_Particle particle;
};

///////////////////////////////////////////////////////////////////////////
/// \class  MassComponent
struct MassComponent final : public mini::ecsComponent<MassComponent> {
    float mass = 0.0F;
};

///////////////////////////////////////////////////////////////////////////
/// \class  BoundingSphereComponent
struct BoundingSphereComponent final
    : public mini::ecsComponent<BoundingSphereComponent> {
    float radius = 1.0F;
};

///////////////////////////////////////////////////////////////////////////
/// \class  BoundingBoxComponent
struct BoundingBoxComponent final
    : public mini::ecsComponent<BoundingBoxComponent> {
    vec2 extents;
};

///////////////////////////////////////////////////////////////////////////
/// \class  MoveableComponent
struct MoveableComponent final : public mini::ecsComponent<MoveableComponent> {
};

#endif // COMPONENTS_HPP