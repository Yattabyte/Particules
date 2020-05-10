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
/// \class  MassComponent
struct MassComponent final : public ecsComponent<MassComponent> {
    float mass = 0.0F;
};

///////////////////////////////////////////////////////////////////////////
/// \class  BoundingSphereComponent
struct BoundingSphereComponent final
    : public ecsComponent<BoundingSphereComponent> {
    float radius = 1.0F;
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